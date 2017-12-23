#ifndef NES_h__
#define NES_h__

#include "Cartridge.h"
#include "CPU.h"
#include "APU.h"
#include "Joypad.h"
#include "PPU.h"
#include "Timer.h"

#include "IRenderWindow.h"
#include "Globals.h"

class NES
{
public:
	uint32 cycles_dma;
	NES(IRenderWindow* graphicsOut);
	int InsertCartridge(Cartridge* cart);
	void EjectCartridge();
	void Reset();
	void Run();
	void Pause();
	void SetRegion(int _region);
	uint16 ReadMemory16(uint16 address);
	uint8 ReadMemory8(uint16 address);
	void WriteMemory16(uint16 address, uint16 val);
	void WriteMemory8(uint16 address, uint8 val);
	~NES();
	int cyclesPerScanline;
	void Update();
	PPU* pPpu;
	Cartridge* pCart;
	IRenderWindow* pWindow;
	
private:
	int region;
	
	bool running;
	bool cartInserted;
	Timer timer;
	
	APU* pApu;
	uint32 cycles_real;
	uint32 cycles_total;
	float cycles_ideal;
	

	CPU* pCpu;
	Joypad* pJoy1;
	Joypad* pJoy2;
	uint8 reg_c1;
	uint8 reg_c2;
	float frameTime;
	bool canReset;
	
};
#endif // NES_h__
