#include "APU_Triangle.h"
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

APU_Triangle::APU_Triangle(double freq)
	: APUChannel(freq), linearCounterLoad(0), triangleHalt(false),
	wave_id(0), linearCounter(0)
{
	uint8 lut[32] =
	{
		0x5*2,0x7f*2,0xA*2,0x1*2,0x14*2,0x2*2,0x28*2,0x3*2,0x50*2,0x4*2,0x1E*2,0x5*2,0x7*2,0x6*2,0x0E*2,0x7*2,
			0x6*2,0x08*2,0xC*2,0x9*2,0x18*2,0xa*2,0x30*2,0xb*2,0x60*2,0xc*2,0x24*2,0xd*2,0x8*2,0xe*2,0x10*2,0xf*2
	};
	memcpy(lengthLut, lut, 32);

	clock_speed = 1789772;


	uint8 heights[32] = { 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
	};

	memcpy(waveHeightLut, heights, 32);
}
APU_Triangle::~APU_Triangle()
{

}
void APU_Triangle::WriteReg0(uint8 val)
{
	holdnote = (val & 0x80) !=0;
	linearCounterLoad = val & 0x7f;
}
void APU_Triangle::WriteReg1(uint8 val)
{
	sweep_shift = val & 3;
}
void APU_Triangle::WriteReg2(uint8 val)
{
	freq = (freq & 0x700) | val;
	UpdateFrequency();
}
void APU_Triangle::WriteReg3(uint8 val)
{
	freq = (freq & 0xff) | ((val & 7) << 8);
	UpdateFrequency();
	vbl_length = lut[(val & 0xf8) >> 3];
	triangleHalt = true;
	//linearCounter = linearCounterLoad;
}
void APU_Triangle::UpdateLinearCounter()
{
	
	//if(!lengthCounterDisable)
	//{
	if(!enabled)
	{
		linearCounter = 0;
	}

	if(triangleHalt)
	{
		linearCounter = linearCounterLoad;
	}
	else if(linearCounter > 0)
		linearCounter--;

	if(!holdnote)
	{
		triangleHalt = false;
	}
	//}
}
void APU_Triangle::UpdateFrequency()
{
	frequency = clock_speed / (freq + 1);
	renderWaveLength = 44100.0 / frequency;
}
void APU_Triangle::UpdateEnvelope()
{

}
void APU_Triangle::UpdateSweep()
{

}
sint16 APU_Triangle::RenderSample()
{
	if((vbl_length > 0) && (linearCounter > 0) && (freq > 0))
	{
		sample_count += 1.0;
		if(sample_count >= renderWaveLength)
		{
			sample_count -= renderWaveLength;
			wave_id++;
		}
		return waveHeightLut[wave_id & 0x1f] * 0x40;
	}
	return 0;
}