#include "APU_DMC.h"
#include "NES.h"
#include <intrin.h>

APU_DMC::APU_DMC(double freq, NES* parent)
	: APUChannel(freq), IRQEnable(true), loop(false), clock_speed(1789772),
	frequency(0.0), renderWaveLength(0.0), dac_counter(0), sample_count(0)
{
	uint32 lut[16] =
		{0xD60, 0xBE0, 0xAA0, 0xA00, 0x8F0, 0x7F0, 0x710, 0x6B0, 0x5F0,
		0x500, 0x470, 0x400, 0x350, 0x2A8, 0x240, 0x1B0 };

	memcpy(dmcWaveLengths, lut, 16*4);

	pNes = parent;
}
APU_DMC::~APU_DMC()
{

}
sint16 APU_DMC::RenderSample()
{
	if(enabled)
	{
		sample_count++;

		if(sample_count > renderWaveLength)
		{
			sample_count -= renderWaveLength;

			if(sampleLength > 0 && shift == 0)
			{
				uint16 tmp;
				tmp = sampleAddress;

				sampleAddress++;

				DAC = pNes->ReadMemory8(tmp);

				sampleLength--;
				shift = 8;

				if(loop && (sampleLength <= 0))
				{
					sampleLength = initialLength;
					sampleAddress = initialAddress;
				}
			}
		}

		if(sampleLength > 0)
		{
			if(DAC !=0)
			{
				int t = DAC & 0x1;
				if( t == 0 && (dac_counter > 1))
				{
					dac_counter -= 2;
				}
				else if(t !=0 && (dac_counter < 0x7e))
				{
					dac_counter += 2;
				}

				if(dac_counter > 0)
				this->dac_counter--;
				else
					dac_counter = 8;

				DAC = DAC >> 1;
				shift--;

				
			}
		}
	}

	return dac_counter * 0x30;
}
void APU_DMC::WriteReg0(uint8 val)
{
	IRQEnable = (val & 0x80) != 0;
	loop = (val & 0x40) !=0;
	freq = dmcWaveLengths[val & 15];
	UpdateFrequency();
}
void APU_DMC::WriteReg1(uint8 val)
{
	DAC = (val & 0x7f);
	shift = 8;
}
void APU_DMC::WriteReg2(uint8 val)
{
	sampleAddress = (val * 0x40) + 0xc000;
	initialAddress = sampleAddress;
}
void APU_DMC::WriteReg3(uint8 val)
{
	sampleLength = (val * 16) + 1;
	initialLength = sampleLength;
}
void APU_DMC::UpdateFrequency()
{
	frequency = clock_speed / (freq + 1);
	renderWaveLength = 44100.0 / frequency;
}