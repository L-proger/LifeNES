#ifndef APU_Square2_h__
#define APU_Square2_h__

#include "APUChannel.h"

class APU_Square2 : public APUChannel
{
public:
	APU_Square2(double freq, int id);
	~APU_Square2();
	void WriteReg0(uint8 val);
	void WriteReg1(uint8 val);
	void WriteReg2(uint8 val);
	void WriteReg3(uint8 val);
	void UpdateEnvelope();
	void UpdateSweep();
	void UpdateLinearCounter(){};
	void UpdateLengthCounter();
	sint16 RenderSample();
private:
	void UpdateFrequency();
	double duty_percent;



	uint32 sweep_delay;
	uint32 sweep_counter;


	bool sweep_inc;
	bool sweep_on;
	double frequency;
	double renderWaveLength;

	uint32 clock_speed;

	sint32 decay_lut[16];
	sint32 vbl_lut[32];
	sint32 num_samples;
	sint32 sweep_phase;
	sint32 phaseacc;
	sint32 cycle_rate;
	uint32 env_vol;
	uint8* lengthLut;
	uint8 channel;
	sint32 freq_limit;
	sint32 duty_flip;
	sint32 env_phase;
	uint8 adder;
	bool has_wave;
	bool decayLoopDisable;
};
#endif // APU_Square2_h__
