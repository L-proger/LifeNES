#include "APU_Square2.h"
#include <intrin.h>

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

static const int duty_lut[4] = { 2, 4, 8, 12 };

/* vblank length table used for rectangles, triangle, noise */
static const uint8 vbl_length_lut[32] =
{
	5, 127,
	10,   1,
	19,   2,
	40,   3,
	80,   4,
	30,   5,
	7,   6,
	13,   7,
	6,   8,
	12,   9,
	24,  10,
	48,  11,
	96,  12,
	36,  13,
	8,  14,
	16,  15
};

static const sint32 freq_limit_lut[8] =
{
	0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0
};


APU_Square2::APU_Square2(double freq, int id)
	: APUChannel(freq), channel(id), has_wave(false), env_phase(0), sweep_phase(0),
	phaseacc(0), sweep_inc(0), sweep_delay(0)
{
	clock_speed = 1789772;

	uint8 lut[32] =
	{
		0x5*2,0x7f*2,0xA*2,0x1*2,0x14*2,0x2*2,0x28*2,0x3*2,0x50*2,0x4*2,0x1E*2,0x5*2,0x7*2,0x6*2,0x0E*2,0x7*2,
		0x6*2,0x08*2,0xC*2,0x9*2,0x18*2,0xa*2,0x30*2,0xb*2,0x60*2,0xc*2,0x24*2,0xd*2,0x8*2,0xe*2,0x10*2,0xf*2
	};
	lengthLut = new uint8[32];
	memcpy(&lengthLut[0], &lut[0], sizeof(uint8) * 32);

	num_samples = 44100 / 60; //NTSC Hardcoded


	for (int i = 0; i < 32; i++)
		vbl_lut[i] = vbl_length_lut[i] * num_samples;



	for(int i = 0; i < 16; i++)
	{
		decay_lut[i] = num_samples * (i + 1);
	}

	cycle_rate = (sint32)(1789772.7272727272727272 * 65536.0 / 44100.0);
}

APU_Square2::~APU_Square2()
{

}
//reg (Volume/Decay)
void APU_Square2::WriteReg0(uint8 val)
{
	//duty == затухание

	//read volume (bit0-3)
	volume = val & 0xf;

	env_delay = decay_lut[volume];


	//envelope_counter++;

	//read envelope decay disabling flag (bit4)
	fixed_envelope = (val & 0x10) !=0;

	//read length counter disabling flag (bit5)
	holdnote = (val & 0x20) !=0;
	//if(holdnote) lengthCounter = 0;

	//read duty cycle type (12,5% 25% 50% 75%)
	duty_flip =  duty_lut[val >> 6];


	//enabled = (volume > 0) && (lengthCounter > 0);
	/*switch(duty)
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
		duty_percent = -0.75;
		break;
	}*/
}
void APU_Square2::WriteReg1(uint8 val)
{

	//sweep right shift amount (bit 0-2)
	sweep_shift = val & 0x7;
	freq_limit = freq_limit_lut[sweep_shift];

	//read sweep direction negate bit 3
	//actually NTSC = 120/(N+1)  PAL=96/(N+1)
	sweep_inc = (val & 0x8) !=0;

	//sweep update rate (bits 4-6)
	sweep_delay = decay_lut[(val >> 4) & 7];

	//is sweep enabled (bit 7)
	sweep_on = (val & 0x80) != 0;
}
void APU_Square2::WriteReg2(uint8 val)
{

	//low 8 bits of wave length!
	freq = (freq & ~0xff) | val;
	//UpdateFrequency();
}
void APU_Square2::WriteReg3(uint8 val)
{

	//3 high bits of 11 bit wave length
	freq =  (freq & 0xff) | ((val & 0x7) << 8);
//	UpdateFrequency();

	//reload length counter
	//if(!holdnote)
	vbl_length = lut[val >> 3];

	//if(!fixed_envelope)
	env_vol = 0;
	adder = 0;
}

void APU_Square2::UpdateLengthCounter()
{
	//if(!holdnote && vbl_length != 0)
	//	vbl_length--;
}

void APU_Square2::UpdateFrequency()
{
	//frequency = clock_speed / (freq + 1) / 16;
	//renderWaveLength = 44100.0 / frequency;
}

void APU_Square2::UpdateSweep()
{
	/*if(sweep_counter > 0)
		sweep_counter--;

	if((sweep_counter == 0) && sweep_on && (sweep_shift !=0) && (vbl_length > 0))
	{
		sweep_counter = sweep_delay;
		//Wavelength = Wavelength +/- (Wavelength SHR S)
		if(freq >= 8)
		{
			sint32 tmp = freq;

			//reverse sweep
			if(sweep_inc)
			{
				if(channel == 0)
					tmp += ~(freq >> sweep_shift);
				else
					tmp -= (freq >> sweep_shift);
			}
			else
			{
				tmp += (freq >> sweep_shift);
			}


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
				sweep_on = false;
			}
		}
	}
	else
	{
		//	sweep_enable = false;
	}

	*/

	/*if(sweep_counter > 0)
		sweep_counter--;

	if((sweep_counter == 0) && sweep_enable && (sweep_shift !=0) && (lengthCounter > 0))
	{
		sweep_counter = sweep_rate;
		//Wavelength = Wavelength +/- (Wavelength SHR S)
		if(waveLength >= 8)
		{
			sint32 tmp = waveLength >> sweep_shift;

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

			tmp += waveLength;

			//check if tmp wave length is greater than 8  (min length)
			//and less that 12 bits (max length is 0x7ff)
			if((tmp < 0x800) && (tmp > 8))
			{
				waveLength = tmp;
				UpdateFrequency();
			}
			else
			{
				//invalid wave length;
				sweep_enable = false;
			}
		}
	}*/
}

void APU_Square2::UpdateEnvelope()
{
	/*if(env_delay > 0)
		env_delay--;




	if(!fixed_envelope && (env_vol > 0) && (env_delay == 0))
	{
		env_vol--;
		env_delay = volume;
	}

	if(holdnote && (env_vol == 0))
	{
		env_vol = 15;
	}*/
}

sint16 APU_Square2::RenderSample()
{
	sint32 out;

	sint32 num_times;
	sint32 total;


	volume -= volume >> 7;

	if(!enabled || vbl_length == 0 || env_delay==0)
		return 0;


	sint32 output;

	if(holdnote)
		vbl_length++;


	env_phase -=4;

	while(env_phase < 0)
	{
		env_phase += env_delay;

		if(holdnote)
			env_vol = (env_vol + 1) & 0x0f;
		else if(env_vol < 0x0f)
			env_vol++;
	}

	if(freq < 8 || (!sweep_inc && (freq > freq_limit)))
		return 0;

	if(sweep_on && sweep_shift)
	{
		sweep_phase -=2;

		while(sweep_phase < 0)
		{
			sweep_phase += sweep_delay;

			if(sweep_inc)
			{
				if(channel == 0)
				{
					freq += ~(freq >> sweep_shift);
				}
				else
				{
					freq -= (freq >> sweep_shift);
				}
			}
			else
			{
				freq += (freq >> sweep_shift);
			}
		}
	}


	phaseacc -= cycle_rate;
	if(phaseacc >=0)
		return 0;

	num_times = total = 0;
	if(fixed_envelope)
	{
		out = volume << 8;
	}
	else
	{
		out = (env_vol ^ 0xf) << 8;
	}




	while(phaseacc < 0)
	{
		phaseacc += (freq + 1) << 16;
		adder = (adder + 1) & 0xf;

		if(adder < duty_flip)
			total += out;
		else
			total -= out;

		num_times++;
	}

	

	return total / num_times;

	/*

	if(lengthCounter > 0)
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
		if(has_wave)
		{
			return 0;
		}

		if(!envelopeDecayDisable)
		{
			return 64 * envelope;
		}
		return 64 * volume;
	}
	return 0;*/
}