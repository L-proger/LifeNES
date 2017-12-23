#include "APU_Noise.h"
#include <intrin.h>
#include <cstring>

APU_Noise::APU_Noise(double freq)
	: APUChannel(freq), envelope(0), noise_shift(1), sample_count(0.0)
{
	uint32 lut1[16] = { 
		4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 
		380, 508, 762, 1016, 2034, 4068 };
		memcpy(&noiseWaveLengthLut[0], lut1, 16 * sizeof(uint32));


	uint8  lut2[32] = {
		0x5*2,0x7f*2,0xA*2,0x1*2,0x14*2,0x2*2,0x28*2,0x3*2,0x50*2,0x4*2,0x1E*2,0x5*2,0x7*2,0x6*2,0x0E*2,0x7*2,
		0x6*2,0x08*2,0xC*2,0x9*2,0x18*2,0xa*2,0x30*2,0xb*2,0x60*2,0xc*2,0x24*2,0xd*2,0x8*2,0xe*2,0x10*2,0xf*2
	};

	memcpy(&lengthValues[0], lut2, 32);

	clock_speed = 1789772;
}
void APU_Noise::UpdateEnvelope()
{
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
APU_Noise::~APU_Noise()
{
	
}
sint16 APU_Noise::RenderSample()
{
	if(vbl_length > 0 && this->enabled)
	{
		sample_count++;
		if(sample_count >= renderWaveLength)
		{
			sample_count -= renderWaveLength;
			uint32 tmp = (noise_shift & 0x4000) >> 14;
			uint32 tmp2;
			if(noiseMode)
			{
				tmp2 = (noise_shift & 0x100) >> 8;
			}
			else
			{
				tmp2 = (noise_shift & 0x2000) >> 13;
			}

			noise_shift = noise_shift << 1;
			noise_shift |= (tmp ^ tmp2) & 1;
		}
		uint32 tmp3 = (noise_shift & 1) * 0x20;
		return tmp3 * (fixed_envelope ? volume : envelope);
	}
	return 0;
}
void APU_Noise::UpdateFrequency()
{
	frequency = clock_speed / (freq + 1) / 16;
	renderWaveLength = 44100.0 / frequency;
}
void APU_Noise::WriteReg0(uint8 val)
{
	volume = env_delay = val & 0xF;
	fixed_envelope = (val & 0x10) != 0;
	holdnote = (val & 0x20) != 0;
}
void APU_Noise::WriteReg1(uint8 val)
{
	freq = noiseWaveLengthLut[val & 15];
	UpdateFrequency();
	noiseMode = (val & 0x80) !=0;
}
void APU_Noise::WriteReg2(uint8 val)
{
	vbl_length = lengthValues[(val & 0xF8) >> 3];
}
void APU_Noise::WriteReg3(uint8 val)
{
	//printf("Write Noise register 3 !\n");
}