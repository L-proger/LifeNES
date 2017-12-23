#include "APU.h"
#include "NES.h"

//APU channels
#include "APU_DMC.h"
#include "APU_Noise.h"
#include "APU_Square.h"
#include "APU_Triangle.h"
#include "APU_Square2.h"

static ALfloat ListenerPos[] = { 0.0f, 0.0f, 0.0f };
static ALfloat ListenerVel[] = { 0.0f, 0.0f, 0.0f };
static ALfloat ListenerOri[] = { 0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f };

APU::APU(NES* parent)
	:reg_status(0), sample_rate(44100.0), frame_counter(0),
	render_frame(0), IRQ_Disable(true), APU_Region(NTSC)
{
	pNes = parent;

	channels[0] = new APU_Square(sample_rate, 0);
	channels[1] = new APU_Square(sample_rate, 1);
	channels[2] = new APU_Triangle(sample_rate);
	channels[3] = new APU_Noise(sample_rate);
	channels[4] = new APU_DMC(sample_rate, pNes);

	//init OpenAL
	dev = alcOpenDevice("Generic Software");
	ALenum error = alGetError();
	if(error!= AL_NO_ERROR)
	{
		printf("Error initializing sound device context!\n");
		return;
	}
	context = alcCreateContext(dev, NULL);
//	if (alGetError() != AL_NO_ERROR)
	//	return AL_FALSE;


	alcMakeContextCurrent(context);
//	if (alGetError() != AL_NO_ERROR)
	//	return AL_FALSE;
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);

	alGenSources(1, &source);

	alSourcef (source, AL_PITCH,    1.0f);
	alSourcef (source, AL_GAIN,    1.0f);
	alSourcefv(source, AL_POSITION,  ListenerPos);
	alSourcefv(source, AL_VELOCITY,  ListenerVel);
	alSourcei (source, AL_LOOPING,  AL_FALSE);

	alGenBuffers(NUM_BUFFERS, &buffers[0]);

	
	memset(data_buffer,0, BUFFER_LENGTH *2);

	for(int i = 0; i < NUM_BUFFERS; i++)
	{
		alBufferData(buffers[i], AL_FORMAT_MONO16, data_buffer,BUFFER_LENGTH*2,  44100);
	}


	alSourceQueueBuffers(source, NUM_BUFFERS, &buffers[0]);


	alSourcePlay(source);
}
APU::~APU()
{

}
static bool a = false;
void APU::Render(uint32 cycles)
{
	UpdateChannels(cycles);


	//render frame samples
	for(int i = 0; i < 735; i++)
	{
		data_buffer[i] = 0;
		for(int j = 0; j < 5; j++)
		{
			if(channels[j]->IsEnabled())
			data_buffer[i]+=channels[j]->RenderSample();
		}
		data_buffer[i] *= 7; 
	}

	

	//Play sound buffer
	ALuint b;
	alSourceUnqueueBuffers(source, 1, &b);

	alBufferData(b, AL_FORMAT_MONO16, data_buffer, BUFFER_LENGTH*2,  44100);
	alSourceQueueBuffers(source, 1, &b);

	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if(state!= AL_PLAYING)
	{
		alSourcePlay(source);
	}

	render_frame++;
}
void APU::WriteConfigRegister(uint8 val)
{
	IRQ_Disable = val & 0x40;
	//APU_Region = ((val & 0x80) !=0) ? PAL : NTSC;
	APU_Region = NTSC;
}
void APU::UpdateChannels(uint32 cycles)
{

	//if PAL region is selected, every 5'th sequence is just skipped
	/*bool clock_skip = (APU_Region == PAL) && ((render_frame % 5) == 4);


	if(!clock_skip)
	{
		channels[0]->UpdateEnvelope();
		channels[0]->UpdateLinearCounter();
		channels[1]->UpdateEnvelope();
		channels[1]->UpdateLinearCounter();
		channels[2]->UpdateEnvelope();
		channels[2]->UpdateLinearCounter();
		channels[3]->UpdateEnvelope();
		channels[3]->UpdateLinearCounter();
		channels[4]->UpdateEnvelope();
		channels[4]->UpdateLinearCounter();
	}


	//clock length counters and sweep units at 120/96 Hz
	if(APU_Region == NTSC)
	{
		uint8 sequence_id = render_frame % 4;
		switch(sequence_id)
		{
		case 1:
		case 3:
			channels[0]->UpdateLengthCounter();
			channels[0]->UpdateSweep();
			channels[1]->UpdateLengthCounter();
			channels[1]->UpdateSweep();
			channels[2]->UpdateLengthCounter();
			channels[2]->UpdateSweep();
			channels[3]->UpdateLengthCounter();
			channels[3]->UpdateSweep();
			channels[4]->UpdateLengthCounter();
			channels[4]->UpdateSweep();
			break;
		}
	}
	else
	{
		uint8 sequence_id = render_frame % 5;
		switch(sequence_id)
		{
		case 0:
		case 2:
			channels[0]->UpdateLengthCounter();
			channels[0]->UpdateSweep();
			channels[1]->UpdateLengthCounter();
			channels[1]->UpdateSweep();
			channels[2]->UpdateLengthCounter();
			channels[2]->UpdateSweep();
			channels[3]->UpdateLengthCounter();
			channels[3]->UpdateSweep();
			channels[4]->UpdateLengthCounter();
			channels[4]->UpdateSweep();
			break;
		}
		printf("FAIL!\n");
	}*/

	//clock envelope and triangle's linear counter at 240 Hz : )



	int frameCount = 4; //NTSC
	//3 if PAL
	bool pal = false;

	while(frameCount > 0)
	{
		frame_counter++;
		int tv = pal ? (frame_counter & 5) : (frame_counter % 4);

		for(int i = 0; i < 5; i++)
		{
			APUChannel* c = channels[i];
			c->UpdateEnvelope();

			switch(tv)
			{
			case 1:
			case 3:
				c->DecrementLengthCounter();
				c->UpdateLinearCounter();
				c->UpdateSweep();
				break;
			}
		}
		frameCount--;
	}
}
uint8 APU::ReadStatus()
{
	return reg_status;
}
void APU::WriteStatus(uint8 val)
{
	reg_status = val;
	for(int i = 0; i < 5; i++)
		channels[i]->SetEnabled((reg_status & (1 << i)) != 0);

}
void APU::WriteChannelReg(uint8 channel, uint8 reg, uint8 val)
{
	switch(reg)
	{
		case 0: channels[channel]->WriteReg0(val); break;
		case 1: channels[channel]->WriteReg1(val); break;
		case 2: channels[channel]->WriteReg2(val); break;
		case 3: channels[channel]->WriteReg3(val); break;
	}
}