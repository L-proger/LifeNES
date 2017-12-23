#ifndef APU_Noise_h__
#define APU_Noise_h__

#include "APUChannel.h"

class APU_Noise : public APUChannel
{
public:
	APU_Noise(double freq);
	~APU_Noise();
	void WriteReg0(uint8 val);
	void WriteReg1(uint8 val);
	void WriteReg2(uint8 val);
	void WriteReg3(uint8 val);
	void UpdateEnvelope();
	void UpdateSweep() {}
	sint16 RenderSample();
	void UpdateLinearCounter(){};
	void UpdateLengthCounter(){}
private:
	void UpdateFrequency();
	sint32 noiseWaveLengthLut[16];
	uint8 lengthValues[32];
	bool noiseMode;
	double frequency;
	double renderWaveLength;
	uint32 clock_speed;
	uint32 envelope;
	uint32 noise_shift;
	double sample_count;
};
#endif // APU_Noise_h__
