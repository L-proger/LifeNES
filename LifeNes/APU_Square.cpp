#include "APU_Square.h"
#include <intrin.h>
#include <cstring>

const uint8 lut[32] =
{
	0x0A, 0xFE, 0x14, 0x02,
	0x28, 0x04, 0x50, 0x06,
	0xA0, 0x08, 0x3C, 0x0A,
	0x0E, 0x0C, 0x1A, 0x0E,
	0x0C, 0x10, 0x18, 0x12,
	0x30, 0x14, 0x60, 0x16,
	0xC0, 0x18, 0x48, 0x1A,
	0x10, 0x1C, 0x20, 0x1E
};



APU_Square::APU_Square(double freq, int id)
	: APUChannel(freq), channel(id), has_wave(false)
{
	clock_speed = 1789772;

	uint8 lut[32] =
	{
		0x5*2,0x7f*2,0xA*2,0x1*2,0x14*2,0x2*2,0x28*2,0x3*2,0x50*2,0x4*2,0x1E*2,0x5*2,0x7*2,0x6*2,0x0E*2,0x7*2,
		0x6*2,0x08*2,0xC*2,0x9*2,0x18*2,0xa*2,0x30*2,0xb*2,0x60*2,0xc*2,0x24*2,0xd*2,0x8*2,0xe*2,0x10*2,0xf*2
	};
	lengthLut = new uint8[32];
	memcpy(&lengthLut[0], &lut[0], sizeof(uint8) * 32);
}
APU_Square::~APU_Square()
{

}
//reg (Volume/Decay)
void APU_Square::WriteReg0(uint8 val)
{
	//duty == затухание

	//read volume (bit0-3)
	volume = env_delay = val & 0xf;
	env_delay++;

	//read envelope decay disabling flag (bit4)
	fixed_envelope = (val & 0x10) !=0;
	
	//read length counter disabling flag (bit5)
	holdnote = (val & 0x20) !=0;
	if(holdnote)
		vbl_length = 0;

	//read duty cycle type (12,5% 25% 50% 75%)
	duty = (val & 0xc0) >> 6;


	//enabled = (volume > 0) && (lengthCounter > 0);
	switch(duty)
	{
	case 0:
		duty_percent = 0.125;
		break;
	case 1:
		duty_percent = 0.25;
		break;
	case 2:
		duty_percent = 0.5;
		break;
	case 3:
		duty_percent = 0.75;
		break;
	}
}
void APU_Square::WriteReg1(uint8 val)
{

	//sweep right shift amount (bit 0-2)
	sweep_shift = val & 0x7;

	//read sweep direction negate bit 3
	//actually NTSC = 120/(N+1)  PAL=96/(N+1)
	sweepDirectionNegate = (val & 0x8) !=0;

	//sweep update rate (bits 4-6)
	sweep_rate = sweep_counter = ((val & 0x70) >> 4) + 1;

	//is sweep enabled (bit 7)
	sweep_enable = (val & 0x80) != 0;
}
void APU_Square::WriteReg2(uint8 val)
{

	//low 8 bits of wave length!
	freq = (freq & 0x700) | val;
	UpdateFrequency();
}
void APU_Square::WriteReg3(uint8 val)
{

	//3 high bits of 11 bit wave length
	freq =  (freq & 0xff) | ((val & 0x7) << 8);
	UpdateFrequency();
	vbl_length = lut[(val & 0xf8) >> 3] * (uint8)(enabled);

	if(!fixed_envelope)
		envelope = 15;
}

void APU_Square::UpdateFrequency()
{
	frequency = clock_speed / (freq + 1) / 16;
	renderWaveLength = 44100.0 / frequency;
}

void APU_Square::UpdateSweep()
{
	if(sweep_counter > 0)
		sweep_counter--;
	
	if((sweep_counter == 0) && sweep_enable && (sweep_shift !=0) && (vbl_length > 0))
	{
		sweep_counter = sweep_rate;
		//Wavelength = Wavelength +/- (Wavelength SHR S)
		if(freq >= 8)
		{
			sint32 tmp = freq >> sweep_shift;

			//reverse sweep
			if(sweepDirectionNegate)
			{
				tmp = -tmp;
				//second square channel
				if(channel == 1)
				{
					tmp--;
				}
			}

			tmp += freq;

			//check if tmp wave length is greater than 8  (min length)
			//and less that 12 bits (max length is 0x7ff)
			if((tmp < 0x800) && (tmp > 8))
			{
				freq = tmp;
				UpdateFrequency();
			}
			else
			{
				//invalid wave length;
				sweep_enable = false;
			}
		}
	}
}

void APU_Square::UpdateEnvelope()
{
	if(env_delay > 0)
		env_delay--;

	


	if(!fixed_envelope && (envelope > 0) && (env_delay == 0))
	{
		envelope--;
		env_delay = volume;
	}

	if(holdnote && (envelope == 0))
	{
		envelope = 15;
	}
}

sint16 APU_Square::RenderSample()
{
	volume -= volume >> 7;

	if(vbl_length > 0)
	{
		sample_count++;
		if(has_wave && (sample_count > (renderWaveLength  * duty_percent)))
		{
			sample_count -= renderWaveLength * duty_percent;
			has_wave = !has_wave;
		}
		else if(!has_wave && (sample_count > (renderWaveLength  * (1.0f - duty_percent))))
		{
			sample_count -= renderWaveLength * (1.0f - duty_percent);
			has_wave = !has_wave;
		}
		if(!has_wave)
		{
			return 0;
		}

		if(!fixed_envelope)
		{
			return 64 * envelope;
		}
		return 64 * volume;
	}
	return 0;

}