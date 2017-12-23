#ifndef APU_DMC_h__
#define APU_DMC_h__

#include "APUChannel.h"

class NES;
class APU_DMC : public APUChannel
{
public:
	APU_DMC(double freq, NES* parent);
	~APU_DMC();
	void WriteReg0(uint8 val);
	void WriteReg1(uint8 val);
	void WriteReg2(uint8 val);
	void WriteReg3(uint8 val);
	void UpdateEnvelope(){}
	void UpdateSweep() {}
	sint16 RenderSample();
	void UpdateLinearCounter(){};
	void UpdateLengthCounter(){}
private:
	void UpdateFrequency();
	bool IRQEnable;
	bool loop;
	uint32 dmcWaveLengths[16];
	uint32 clock_speed;
	double frequency;
	double renderWaveLength;
	uint8 DAC;
	uint32 shift;
	uint16 sampleAddress;
	uint16 initialAddress;

	sint32 sampleLength;
	uint32 initialLength;
	uint32 dac_counter;
	double sample_count;
	NES* pNes;
};
#endif // APU_DMC_h__
