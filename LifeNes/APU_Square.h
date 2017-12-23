#ifndef APU_Square_h__
#define APU_Square_h__

#include "APUChannel.h"

class APU_Square : public APUChannel
{
public:
	APU_Square(double freq, int id);
	~APU_Square();
	void WriteReg0(uint8 val);
	void WriteReg1(uint8 val);
	void WriteReg2(uint8 val);
	void WriteReg3(uint8 val);
	void UpdateEnvelope();
	void UpdateSweep();
	void UpdateLinearCounter(){};
	void UpdateLengthCounter(){}
	sint16 RenderSample();
private:
	void UpdateFrequency();
	double duty_percent;
	uint8 duty;
	
	
	
	uint32 sweep_rate;
	uint32 sweep_counter;
	
	
	bool sweepDirectionNegate;
	bool sweep_enable;
	double frequency;
	double renderWaveLength;
	
	uint32 clock_speed;
	
	uint32 envelope;
	uint8* lengthLut;
	uint8 channel;
	
	bool has_wave;
	bool decayLoopDisable;
};
#endif // APU_Square_h__
