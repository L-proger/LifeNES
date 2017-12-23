#ifndef APU_Triangle_h__
#define APU_Triangle_h__


#include "APUChannel.h"

class APU_Triangle : public APUChannel
{
public:
	APU_Triangle(double freq);
	~APU_Triangle();
	void WriteReg0(uint8 val);
	void WriteReg1(uint8 val);
	void WriteReg2(uint8 val);
	void WriteReg3(uint8 val);
	void UpdateEnvelope();
	void UpdateSweep();
	void UpdateLinearCounter();
	void UpdateLengthCounter(){}
	sint16 RenderSample();
private:
	sint32 linearCounterLoad;
	void UpdateFrequency();
	bool triangleHalt;
	uint8 lengthLut[32];
	sint32 linearCounter;
	double frequency;
	double renderWaveLength;
	uint32 clock_speed;
	uint32 wave_id;
	uint8 waveHeightLut[32];
};
#endif // APU_Triangle_h__
