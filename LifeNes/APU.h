#ifndef APU_h__
#define APU_h__

#include "Globals.h"
#include "APUChannel.h"

//OpenAL
#include <al.h>
#include <alc.h>
#define  NUM_BUFFERS 5
#define BUFFER_LENGTH (735)

#define APU_QUEUE_SIZE 4096

struct apudata
{
	uint32 timestamp, address;
	uint8 value;
};


class NES;
class APU
{
public:
	APU(NES* parent);
	~APU();
	void Render(uint32 cycles);

	// $4015
	uint8 ReadStatus();
	void WriteStatus(uint8 val);
	void WriteConfigRegister(uint8 val);
	void WriteChannelReg(uint8 channel, uint8 reg, uint8 val);
private:
	void UpdateChannels(uint32 cycles);
	NES* pNes;
	//square x2; triangle x1; noise x1; DMC x1;
	APUChannel* channels[5];
	uint8 reg_status;
	double sample_rate;
	uint32 frame_counter;

	ALCdevice* dev;
	ALCcontext* context;
	ALuint source;
	ALuint buffers[NUM_BUFFERS];
	uint32 render_frame;
	sint16 data_buffer[BUFFER_LENGTH];  //44100 / 60  (NTSC)
	bool IRQ_Disable;
	int APU_Region;


	apudata queue[APU_QUEUE_SIZE];
};
#endif // APU_h__
