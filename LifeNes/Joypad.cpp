#include "Joypad.h"
#include "NES.h"
#include <Windows.h>

Joypad::Joypad(NES* parent)
	:strobe(0), buttonState(0), offset(0)
{
	pNes = parent;

	names = new char[8*10];
	memset(names, 0, 8*10);
	memcpy(names + (10 * 0), "A", 1);
	memcpy(names + (10 * 1), "B", 1);
	memcpy(names + (10 * 2), "SELECT", 6);
	memcpy(names + (10 * 3), "START", 5);
	memcpy(names + (10 * 4), "UP", 2);
	memcpy(names + (10 * 5), "DOWN", 4);
	memcpy(names + (10 * 6), "LEFT", 4);
	memcpy(names + (10 * 7), "RIGHT", 5);

	keyMap[JK_A] = 'Z';
	keyMap[JK_B] = 'X';
	keyMap[JK_SELECT] = 'C';
	keyMap[JK_START] = 'V';
	keyMap[JK_UP] = VK_UP;
	keyMap[JK_DOWN] = VK_DOWN;
	keyMap[JK_LEFT] = VK_LEFT;
	keyMap[JK_RIGHT] = VK_RIGHT;
}
Joypad::~Joypad()
{

}
void Joypad::MapButton(int joyKey, int keyboradButton)
{

}
uint8 Joypad::ReadState()
{
	//printf("ReadButton\n");
	//Sleep(10);
	
	uint8 val = 0;
	
	if(offset > 0x7)
		val = 0x40;
	else
	{
		val = buttonState & (0x1);
		buttonState >>= 1;
		val |= 0x40;
	}

	offset++;
	return val;
	
}
void Joypad::WriteState(uint8 val)
{
	if(strobe == 0x1 && ((val & 0x1) == 0x0))
	{
		//get input state and assign bits!
		//0-7 bits = A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
		buttonState = 0x0; //заглушка (кнопки не нажаты)

		for(int i = 0; i < 8; i++)
		{
			int ck = keyMap[i];
			if(GetKeyState(ck) < 0)
				buttonState = buttonState | (0x1 << i);
		}

		offset = 0;
		//printf("JOYPAD STROBE!\n");
		//(50);
	}
	strobe = (val & 0x1); //setup strobe state
}