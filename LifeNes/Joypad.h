#ifndef Joypad_h__
#define Joypad_h__

#include "Globals.h"
enum JOYPAD_KEY
{
	JK_A = 0,
	JK_B = 1,
	JK_SELECT = 2,
	JK_START = 3,
	JK_UP = 4,
	JK_DOWN = 5,
	JK_LEFT = 6,
	JK_RIGHT = 7,

};

class NES;
class Joypad
{
public:
	Joypad(NES* parent);
	~Joypad();
	uint8 ReadState();
	void WriteState(uint8 val);
	void MapButton(int joyKey, int keyboradButton);
private:
	int keyMap[8];
	uint8 strobe;
	uint8 buttonState;
	NES* pNes;
	uint8 offset;
	char* names;
};
#endif // Joypad_h__
