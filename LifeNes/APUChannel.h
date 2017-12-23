#ifndef APUChannel_h__
#define APUChannel_h__
#include "Globals.h"
#include <stdio.h>

class APUChannel
{
public:
	APUChannel(double _samplingRate)
		:sample_count(0.0),volume(0), env_delay(0)
	{
		sample_rate = _samplingRate;
		enabled = false;
		volume = 6;
	}
	virtual ~APUChannel(){}
	void SetEnabled(bool state) 
	{
		enabled = state;
		if(state == false)
		vbl_length = 0;
	}
	bool IsEnabled() {return enabled;}
	virtual void WriteReg0(uint8 val) = 0;
	virtual void WriteReg1(uint8 val) = 0;
	virtual void WriteReg2(uint8 val) = 0;
	virtual void WriteReg3(uint8 val) = 0;
	virtual void UpdateLinearCounter() = 0;
	virtual void UpdateLengthCounter() = 0;
	virtual void UpdateEnvelope() = 0;
	void DecrementLengthCounter()
	{
		if(!holdnote)
		{
			if(vbl_length > 0)
			vbl_length--;
		}
		if(!enabled)
		{
			vbl_length = 0;
		}
	}
	virtual void UpdateSweep() = 0;
	virtual sint16 RenderSample() = 0;
protected:
	uint32 volume;
	uint32 env_delay;
	bool fixed_envelope;
	double sample_rate;
	bool enabled;
	bool holdnote;
	uint32 sweep_shift;
	sint32 freq;
	uint32 vbl_length;
	double sample_count;
};
#endif // APUChannel_h__
