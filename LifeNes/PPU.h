#ifndef PPU_h__
#define PPU_h__
#include "Globals.h"
#include <Windows.h>
#include "Timer.h"

class NES;
class PPU
{
public:
	PPU(NES* parent);
	~PPU();
	void WriteControlRegister1(uint8 data);
	void WriteControlRegister2(uint8 data);
	void WriteVramAddress1(uint8 val);
	void WriteVramAddress2(uint8 val);
	void WriteSpriteRamAddress(uint8 val);
	void WriteSpriteDMA(uint8 val);
	void RenderScanline(uint16 scanId);
	uint8 ReadStatusRegister();
	uint8 ReadVRAM();
	uint8 ReadSpriteRAM();
	void WriteSpriteRAM(uint8 val);
	void WriteVRAM(uint8 data);
	void Present();
	uint16 ScanPerFrame;
	uint16 ScanPerNMI;
	bool IsNmiEnabled();
	void SetVblank(bool state);
	bool GetVblank();
	void RenderBuffers();

	void BeginFrame();

	void EndFrame();
	bool CanNMI()
	{
		return (!doneNmi && vblank_nmi && inVBlank);
	}
	void OnNMI()
	{
		doneNmi = true;
	}
private:
	uint16 latch_vram2;
	bool spriteOverflow;
	bool doneNmi;
	void RenderSprites(bool spriteOnTop);
	void RenderBackground();
	NES* pNes;
	uint8* palette_ram;
	uint8* video_ram;
	uint8* sprite_ram;
	uint16 nameTableAddress;
	uint8 addressIncrement;
	uint16 spritePatternTbl;
	uint16 backgroundPatternTbl;
	uint8 spriteSize;
	bool state_master;
	bool vblank_nmi;
	bool colorMode;
	bool clipBackground;
	bool clipSprite;
	bool hideBackground;
	bool hideSprites;
	uint8 backgroundColor;
	uint16 scanIndex;
	bool sprite0hit;
	bool inVBlank;
	uint8 spritePerScanline;
	uint8 readBuffer;
	uint8 reg_vramAddress1;
	uint16 reg_vramAddress2;
	uint16 reg_vramAddress2_back;
	uint8 reg_spriteRamAddress;

	bool writeSwitch;
	bool writeEnable;

	uint8 scrollV;
	uint8 scrollH;

	BITMAPINFOHEADER bitmapInfo;
	BITMAPINFO info;
	HDC context;

	uint32 frameCounter;
	uint32* backBuffer;
	uint16* backBufferSrcSprite;
	uint16* backBufferSrcBack;
	uint32 screenWidth;
	uint32 screenHeight;

	uint32 scanHit;
	
	uint16 colorPalette[64];
};
#endif // PPU_h__
