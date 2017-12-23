#include "PPU.h"
#include "NES.h"

PPU::PPU(NES* parent)
	:sprite0hit(false), scanIndex(0), spritePerScanline(0), inVBlank(false),
	writeSwitch(false), nameTableAddress(0), addressIncrement(1),spriteSize(8),state_master(true),
	reg_vramAddress1(0), reg_vramAddress2(0), reg_spriteRamAddress(0), writeEnable(false), frameCounter(0),
	readBuffer(0), latch_vram2(0), spriteOverflow(false)
{
	pNes = parent;

	screenWidth = pNes->pWindow->GetWidth();
	screenHeight = pNes->pWindow->GetHeight();
	

	::ZeroMemory(&bitmapInfo,sizeof(BITMAPINFOHEADER));
	bitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.biWidth = pNes->pWindow->GetWidth();
	bitmapInfo.biHeight = -pNes->pWindow->GetHeight();
	bitmapInfo.biPlanes = 1;
	bitmapInfo.biBitCount = 32;
	bitmapInfo.biCompression = 0;
	bitmapInfo.biSizeImage = screenWidth * screenHeight * 4;

	info.bmiHeader = bitmapInfo;

	context = ::GetDC((HWND)pNes->pWindow->GetWindowHandle());

	backBuffer = new uint32[screenWidth * screenHeight];
	backBufferSrcSprite = new uint16[256*240];
	backBufferSrcBack = new uint16[256*240];

	memset(backBuffer, 0, screenWidth * screenHeight * 4);
	memset(backBufferSrcSprite, 0, 256*240 * 2);
	memset(backBufferSrcBack, 0, 256*240 * 2);

	video_ram = new uint8[0x8000];
	palette_ram = new uint8[32];
	sprite_ram = new uint8[256];


	memset(video_ram, 0x0, 0x8000);
	memset(palette_ram, 0x0, 32);
	memset(sprite_ram, 0x0, 256);

	


	uint16 colorPaletteTmp[64] = {
		0x8410, 0x17, 0x3017, 0x8014, 0xb80d, 0xb003, 0xb000, 0x9120,
		0x7940, 0x1e0, 0x241, 0x1e4, 0x16c, 0x1, 0x20, 0x20,
		0xce59, 0x2df, 0x41ff, 0xb199, 0xf995, 0xf9ab, 0xf9a3, 0xd240,
		0xc300, 0x3bc0, 0x1c22, 0x4ac, 0x438, 0x1082, 0x841, 0x841,
		0xffff, 0x4bf, 0x6c3f, 0xd37f, 0xfbb9, 0xfb73, 0xfbcb, 0xfc8b,
		0xfd06, 0xa5e0, 0x56cd, 0x4eb5, 0x6df, 0x632c, 0x861, 0x861,
		0xffff, 0x85ff, 0xbddf, 0xd5df, 0xfdfd, 0xfdf9, 0xfe36, 0xfe75,
		0xfed4, 0xcf13, 0xaf76, 0xafbd, 0xb77f, 0xdefb, 0x1082, 0x1082
	};
	memcpy(&colorPalette[0], &colorPaletteTmp[0], sizeof(uint16)*64);

}
void PPU::Present()
{
	SetDIBitsToDevice(context, 0,0, screenWidth, screenHeight, 0, 0, 0, screenHeight, backBuffer, &info, 0);
	//pNes->pWindow->Update();
}
PPU::~PPU()
{

}
void PPU::WriteControlRegister1(uint8 data)
{
	//bit 0, 1
	if(!clipBackground)
	{
		if(!hideBackground)
		{
			switch(data & 0x3)
			{
			case 0x0: nameTableAddress = 0x2000; break;
			case 0x1: nameTableAddress = 0x2400; break;
			case 0x2: nameTableAddress = 0x2800; break;
			case 0x3: nameTableAddress = 0x2C00; break;
			}
		}
		
	}
	
	//bit 2
	if(data & (0x1 << 2))
		addressIncrement = 32;
	else
		addressIncrement = 1;

	//bit 3
	if(data & (0x1 << 3))
		spritePatternTbl = 0x1000;
	else
		spritePatternTbl = 0x0000;

	//bit 4
	if(data & (0x1 << 4))
		backgroundPatternTbl = 0x1000;
	else
		backgroundPatternTbl = 0x0000;

	//bit5 
	if(data & (0x1 << 5))
		spriteSize = 16;
	else
		spriteSize = 8;

	//bit 6 (master/slave)   //CHECK ME o_O
	if(data & (0x1 << 6))
		state_master = false;
	else
		state_master = true;

	//bit 7 (disable/enable  NMI upon v-blank)
	if(data & (0x1 << 7))
	{
		if(vblank_nmi == false)
			doneNmi = false;
		vblank_nmi = true;
	}
	else
		vblank_nmi = false;
}

void PPU::BeginFrame()
{
	doneNmi = false;
}

void PPU::EndFrame()
{

}

void PPU::WriteControlRegister2(uint8 data)
{
	//bit 0  (color/monochrome mode)
	colorMode = !(data & 0x1);

	//bit 1
	if(data & (0x1 << 1))
		clipBackground = false;
	else
		clipBackground = true;

	//bit2
	if(data & (0x1 << 2))
		clipSprite = false;
	else
		clipSprite = true;

	//bit 3
	if(data & (0x1 << 3))
		hideBackground = false;
	else
		hideBackground = true;

	//bit 4
	if(data & (0x1 << 4))
		hideSprites = false;
	else
		hideSprites = true;

	backgroundColor = data >> 5;
}
uint8 PPU::ReadStatusRegister()
{
	uint8 val = 0;


	//bit 4  (if set - disable VRAM writes)  dunno how it works : 

	//if(!writeEnable)
	//val = val | (0x1 << 4);

	//bit 5  Lost Sprites flag may become set only if video is enabled (ie. BG or OBJ must be on)
	//if(!hideBackground || !clipSprite || !hideSprites)
	if(spriteOverflow)
		val = val | (0x1 << 5);

	//bit 6
	if(sprite0hit)
		val = val | (0x1 << 6);

	//bit 7 (indicate VBlank)
	if(inVBlank)
		val = val | (0x1 << 7);


	writeSwitch = false;
	//latch_vram2 = 0;

	inVBlank = false;

	return val;
}
void PPU::WriteVramAddress1(uint8 val)
{
	if(writeSwitch) //second write
	{
		scrollH = val;
		if(scrollH > 239)
			scrollH = 0;
		

	}
	else
	{
		scrollV = val;
	}
	writeSwitch = !writeSwitch;
	//reg_vramAddress1 = val;
}
void PPU::WriteVramAddress2(uint8 val)
{
	if(writeSwitch) //second write
	{
		latch_vram2 = (latch_vram2 & 0x7f00) | val;
		reg_vramAddress2 = latch_vram2;

		if ((reg_vramAddress2_back == 0) && (scanIndex < 240))
		{
			//We may have a scrolling trick
			if ((reg_vramAddress2 >= 0x2000) && (reg_vramAddress2 <= 0x2400))
				scrollH = (byte)(((reg_vramAddress2 - 0x2000) / 0x20) * 8 - scanIndex);
		}
		
	}
	else //first write
	{
		reg_vramAddress2_back = reg_vramAddress2;
		latch_vram2 = (latch_vram2 & 0xff) | (uint16(val & 0x3f) << 8);
	}

	writeSwitch = !writeSwitch;
}
void PPU::WriteSpriteRamAddress(uint8 val)
{
	reg_spriteRamAddress = val;
}
void PPU::WriteSpriteDMA(uint8 val)
{
	uint16 offset = uint16(val) * 0x100;
	//copy 256 byte SPRITE-RAM page
	//printf("SPR-RAM DMA! Starts at:%i\n", reg_spriteRamAddress);
	for(int i = 0; i < 0x100; i++)
	{
		uint16 offset2 = (reg_spriteRamAddress + i) & 0xff;
		sprite_ram[offset2] = pNes->ReadMemory8(offset + i);
	}
}
uint8 PPU::ReadSpriteRAM()
{
	return sprite_ram[reg_spriteRamAddress];
}
void PPU::WriteSpriteRAM(uint8 val)
{
	sprite_ram[reg_spriteRamAddress] = val;
	reg_spriteRamAddress++;
}
uint8 PPU::ReadVRAM()
{	
	uint8 val = 0;
	
	uint16 readAddress = reg_vramAddress2 % 0x4000;


	if(readAddress < 0x2000) //pattern table
	{
		val = readBuffer;
		readBuffer = pNes->pCart->ReadChrRom(readAddress);
	}
	else if(readAddress < 0x3f00) //name tables
	{
		uint16 mirror = readAddress;
		val = readBuffer;

		if(mirror >= 0x3000)
		{
			mirror = 0x2000 + mirror % 0x3000;
		}


		
			if (pNes->pCart->mirror_mode == MIRROR_HORIZONTAL)
			{
				switch (mirror & 0x2C00)
				{
				case (0x2000): readBuffer = video_ram[mirror];
					break;
				case (0x2400): readBuffer = video_ram[(mirror - 0x400)];
					break;
				case (0x2800): readBuffer = video_ram[mirror - 0x400];
					break;
				case (0x2C00): readBuffer = video_ram[(mirror - 0x800)];
					break;
				}
			}
			else if (pNes->pCart->mirror_mode == MIRROR_VERTICAL)
			{
				switch (mirror & 0x2C00)
				{
				case (0x2000): readBuffer = video_ram[mirror];
					break;
				case (0x2400): readBuffer = video_ram[mirror];
					break;
				case (0x2800): readBuffer = video_ram[mirror - 0x800];
					break;
				case (0x2C00): readBuffer = video_ram[(mirror - 0x800)];
					break;
				}
			}
			else if (pNes->pCart->mirror_mode == 5)
			{
				PRINT("OMG!!11222\n");
				/*if (myEngine.myCartridge.mirroringBase == 0x2000)
				{
					switch (vramReadWriteAddress & 0x2C00)
					{
					case (0x2000): nameTables[vramReadWriteAddress - 0x2000] = data;
						break;
					case (0x2400): nameTables[vramReadWriteAddress - 0x400 - 0x2000] = data;
						break;
					case (0x2800): nameTables[vramReadWriteAddress - 0x800 - 0x2000] = data;
						break;
					case (0x2C00): nameTables[vramReadWriteAddress - 0xC00 - 0x2000] = data;
						break;
					}
				}
				else if (myEngine.myCartridge.mirroringBase == 0x2400)
				{
					switch (vramReadWriteAddress & 0x2C00)
					{
					case (0x2000): nameTables[vramReadWriteAddress + 0x400 - 0x2000] = data;
						break;
					case (0x2400): nameTables[vramReadWriteAddress - 0x2000] = data;
						break;
					case (0x2800): nameTables[vramReadWriteAddress - 0x400 - 0x2000] = data;
						break;
					case (0x2C00): nameTables[vramReadWriteAddress - 0x800 - 0x2000] = data;
						break;
					}
				}*/
			}
			//four screen mirroring
			else
			{
				readBuffer = video_ram[mirror];
			}




		//readBuffer = video_ram[reg_vramAddress2];
	}
	else if(readAddress < 0x4000) //palettes + mirrors
	{
	//	printf("Read palette  at address 0x%p\n", readAddress);
	//	Sleep(3000);
		uint16 addr = readAddress % 0x20 + 0x3F00;
		val = video_ram[addr];



		uint16 vramVal = addr % 0x1000 + 0x2000;



		
			if (pNes->pCart->mirror_mode == MIRROR_HORIZONTAL)
			{
				switch (vramVal & 0x2C00)
				{
				case (0x2000): readBuffer = video_ram[vramVal];
					break;
				case (0x2400): readBuffer = video_ram[(vramVal - 0x400)];
					break;
				case (0x2800): readBuffer = video_ram[vramVal - 0x400];
					break;
				case (0x2C00): readBuffer = video_ram[(vramVal - 0x800)];
					break;
				}
			}
			else if (pNes->pCart->mirror_mode == MIRROR_VERTICAL)
			{
				switch (addr & 0x2C00)
				{
				case (0x2000): readBuffer = video_ram[vramVal];
					break;
				case (0x2400): readBuffer = video_ram[vramVal];
					break;
				case (0x2800): readBuffer = video_ram[vramVal - 0x800];
					break;
				case (0x2C00): readBuffer = video_ram[(vramVal - 0x800)];
					break;
				}
			}
			else if (pNes->pCart->mirror_mode == 5)
			{
				PRINT("OMG!!11222\n");
				/*if (myEngine.myCartridge.mirroringBase == 0x2000)
				{
					switch (vramReadWriteAddress & 0x2C00)
					{
					case (0x2000): nameTables[vramReadWriteAddress - 0x2000] = data;
						break;
					case (0x2400): nameTables[vramReadWriteAddress - 0x400 - 0x2000] = data;
						break;
					case (0x2800): nameTables[vramReadWriteAddress - 0x800 - 0x2000] = data;
						break;
					case (0x2C00): nameTables[vramReadWriteAddress - 0xC00 - 0x2000] = data;
						break;
					}
				}
				else if (myEngine.myCartridge.mirroringBase == 0x2400)
				{
					switch (vramReadWriteAddress & 0x2C00)
					{
					case (0x2000): nameTables[vramReadWriteAddress + 0x400 - 0x2000] = data;
						break;
					case (0x2400): nameTables[vramReadWriteAddress - 0x2000] = data;
						break;
					case (0x2800): nameTables[vramReadWriteAddress - 0x400 - 0x2000] = data;
						break;
					case (0x2C00): nameTables[vramReadWriteAddress - 0x800 - 0x2000] = data;
						break;
					}
				}*/
			}
			//four screen mirroring
			else
			{
				readBuffer = video_ram[vramVal];
			}

	}
	else
	{
		printf("SHIT!!  SHOULD HANDLE MIRRORING! >_<\n");
		val = video_ram[reg_vramAddress2];

	}
	//printf("Read VRAM at address: 0x%p\tValue:%i\n", reg_vramAddress2, val);
	//Sleep(3900);

	reg_vramAddress2 += addressIncrement;
	return val;
}
void PPU::WriteVRAM(uint8 data)
{
	uint16 writeAddress = reg_vramAddress2 % 0x4000;
	//writeAddress = writeAddress % 0x4000;
	//printf("Write VRAM at address: 0x%p\tValue:%i\n", reg_vramAddress2, data);
	//	Sleep(3900);

		if(writeAddress < 0x2000)
		{
			if(pNes->pCart->hasVRAM)
			{
				pNes->pCart->chr_rom[writeAddress] = data;
			}
		}
		else if(writeAddress < 0x3f00)
		{
			uint16 mirror = writeAddress;


			if(mirror >= 0x3000)
			{
				mirror = 0x2000 + mirror % 0x3000;
			}

			if (pNes->pCart->mirror_mode == MIRROR_HORIZONTAL)
			{
				switch (mirror & 0x2C00)
				{
				case (0x2000): video_ram[mirror] = data;
					break;
				case (0x2400): video_ram[(mirror - 0x400)] = data;
					break;
				case (0x2800): video_ram[mirror - 0x400] = data;
					break;
				case (0x2C00): video_ram[(mirror - 0x800)] = data;
					break;
				}
			}
			else if (pNes->pCart->mirror_mode == MIRROR_VERTICAL)
			{
				switch (mirror & 0x2C00)
				{
				case (0x2000): video_ram[mirror] = data;
					break;
				case (0x2400): video_ram[mirror] = data;
					break;
				case (0x2800): video_ram[mirror - 0x800] = data;
					break;
				case (0x2C00): video_ram[(mirror - 0x800)] = data;
					break;
				}
			}
			else if (pNes->pCart->mirror_mode == 5)
			{
				PRINT("OMG!!11222\n");
				/*if (myEngine.myCartridge.mirroringBase == 0x2000)
				{
					switch (vramReadWriteAddress & 0x2C00)
					{
					case (0x2000): nameTables[vramReadWriteAddress - 0x2000] = data;
						break;
					case (0x2400): nameTables[vramReadWriteAddress - 0x400 - 0x2000] = data;
						break;
					case (0x2800): nameTables[vramReadWriteAddress - 0x800 - 0x2000] = data;
						break;
					case (0x2C00): nameTables[vramReadWriteAddress - 0xC00 - 0x2000] = data;
						break;
					}
				}
				else if (myEngine.myCartridge.mirroringBase == 0x2400)
				{
					switch (vramReadWriteAddress & 0x2C00)
					{
					case (0x2000): nameTables[vramReadWriteAddress + 0x400 - 0x2000] = data;
						break;
					case (0x2400): nameTables[vramReadWriteAddress - 0x2000] = data;
						break;
					case (0x2800): nameTables[vramReadWriteAddress - 0x400 - 0x2000] = data;
						break;
					case (0x2C00): nameTables[vramReadWriteAddress - 0x800 - 0x2000] = data;
						break;
					}
				}*/
			}
			//four screen mirroring
			else
			{
				video_ram[mirror] = data;
			}
		}
		else if(writeAddress >= 0x3f00 && writeAddress < 0x4000) //name tables
		{
			uint16 addr = reg_vramAddress2 % 0x20 + 0x3F00;
			video_ram[addr] = data;
			if ((addr & 0x7) == 0) //WTF!!!!
			{
				video_ram[(addr) ^ 0x10] = data;
			}
		}
		else
		{
			printf("Wrong VRAM address write! Maybe need mirroring here...\n");
		}
		//video_ram[reg_vramAddress2] = data;
		reg_vramAddress2 += addressIncrement;
	
}
void PPU::RenderSprites(bool spriteOnTop)
{
	uint16 scanId = scanIndex;
	int topVal = spriteOnTop == true ? 0x0 : 0x20;
	//scan all 64 sprites in SPR-RAM back to front
	for(int i = 63; i >= 0; i--)
	{
		uint8 memOffset = i * 4;
		uint8 yCoord = sprite_ram[memOffset] + 1;
		uint8 tileNum = sprite_ram[memOffset + 1];
		uint8 spriteFlags = sprite_ram[memOffset + 2];
		uint8 xCoord = sprite_ram[memOffset + 3];
		
		//if "ray beam" intersects our sprite and correct priority
		if( ((spriteFlags & 0x20) == topVal) && yCoord <= scanId && (yCoord + spriteSize) > scanId)
		{
			uint8 lineId;
			uint16 chrOffset;
			scanHit++;
			//render 8x8 and 8x16 sprites differently
			if(spriteSize == 8)
			{
				//handle vertical flip flag
				if(spriteFlags & 0x80) //flip vertical
					lineId = yCoord + 7 - scanId;
				else //normal rendering
					lineId = scanId - yCoord;

				//get memory offset in CHR-ROM for current tile
				//data for each tile is 16 bytes!
				chrOffset = spritePatternTbl + tileNum * 16;

				uint8 chrPlane1 = pNes->pCart->ReadChrRom(chrOffset + lineId);
				uint8 chrPlane2 = pNes->pCart->ReadChrRom(chrOffset + lineId + 8);

				uint32 paletteHighBits = uint32(spriteFlags & 0x3) << 2;

				uint32 paletteOffset = 0;
				
				for(int x = 0; x < 8; x++)
				{
					//handle horizontal flip
					if(spriteFlags & 0x40)
					{
						paletteOffset = paletteHighBits | 
							(((chrPlane2 & (0x1 << x)) >> x) << 1) |
							((chrPlane1 & (0x1 << x)) >> x);

					}
					else
					{
						paletteOffset = paletteHighBits | 
							(((chrPlane2 & (0x1 << (7 - x))) >> (7 - x)) << 1) |
							((chrPlane1 & (0x1 << (7 - x))) >> (7 - x));
					}

					//if color !=0  (NOT transparent) - draw it
					if(paletteOffset % 4)
					{
						int realX = xCoord + x;
						int realY = scanId;

						if(realX < 256)
						{
							uint8* ptr = video_ram + 0x3f10 + paletteOffset;
							uint8 val1 = ptr[0];
							uint8 off = (0x3f & val1);
							uint16 pv = colorPalette[off];


							bool backTransparent = backBufferSrcBack[realY * 256 + realX] == 0;

							if(!spriteOnTop)
							{
								if(backTransparent)
								backBufferSrcSprite[realY * 256 + realX] = pv;
							}
							else
							{
								backBufferSrcSprite[realY * 256 + realX] = pv;
							}
							
							//if(scanId < 239)
							//{


								//sprite0hit
								if(clipBackground | clipSprite)
								{
									if(realX >=8 && i == 0 && realX < 255 && !hideBackground && !backTransparent)
										sprite0hit = true;
								}
								else if(i == 0 && realX < 255 && !hideBackground && !backTransparent)
								{
									sprite0hit = true;
								}
						//	}
						}

						
						
					}

					/*if(y>=0 && y < 256 && x>=0 && y <240)
					{
						backBuffer[y * 256 + x] = 0x00ff0000;
					}*/
				}
			




				
			}
			else //8x16 sprite
			{
				//handle vertical flip
				if(spriteFlags & 0x80)
				{
					lineId = yCoord + 15 - scanId;
				}
				else
				{
					lineId = scanId - yCoord;
				}

				//get pattern table offset
				//even sprite is in 0x0000
				//odd  sprite is in 0x1000
				if(lineId < 8) //top tile
				{
					if((tileNum % 2) == 0)
					{
						chrOffset = 0x0000 + (tileNum) * 16;
					}
					else
					{
						chrOffset = 0x1000 + (tileNum - 1) * 16;
					}
				}
				else // bottom tile
				{
					lineId -= 8;

					if((tileNum % 2) == 0)
					{
						chrOffset = 0x0000 + (tileNum + 1) * 16;
					}
					else
					{
						chrOffset = 0x1000 + (tileNum) * 16;
					}
				}


				uint8 chrPlane1 = pNes->pCart->ReadChrRom(chrOffset + lineId);
				uint8 chrPlane2 = pNes->pCart->ReadChrRom(chrOffset + lineId + 8);

				uint32 paletteHighBits = uint32(spriteFlags & 0x3) << 2;

				uint32 paletteOffset = 0;





				for(int x = 0; x < 8; x++)
				{
					//handle horizontal flip
					if(spriteFlags & 0x40)
					{
						paletteOffset = paletteHighBits | 
							(((chrPlane2 & (0x1 << x)) >> x) << 1) |
							((chrPlane1 & (0x1 << x)) >> x);

					}
					else
					{
						paletteOffset = paletteHighBits | 
							(((chrPlane2 & (0x1 << (7 - x))) >> (7 - x)) << 1) |
							((chrPlane1 & (0x1 << (7 - x))) >> (7 - x));
					}

					//if color !=0  (NOT transparent) - draw it
					if(paletteOffset % 4)
					{
						int realX = xCoord + x;
						int realY = scanId;

						if(realX < 256)
						{
							uint8* ptr = video_ram + 0x3f10 + paletteOffset;
							uint8 val1 = ptr[0];
							uint8 off = (0x3f & val1);
							uint16 pv = colorPalette[off];


							bool backTransparent = backBufferSrcBack[realY * 256 + realX] == 0;

							if(!spriteOnTop)
							{
								if(backTransparent)
								backBufferSrcSprite[realY * 256 + realX] = pv;
							}
							else
							{
								backBufferSrcSprite[realY * 256 + realX] = pv;
							}
							
							//if(scanId < 239)
							//{


								//sprite0hit
								if(clipBackground | clipSprite)
								{
									if(realX >=8 && i == 0 && realX < 255 && !hideBackground && !backTransparent)
										sprite0hit = true;
								}
								else if(i == 0 && realX < 255 && !hideBackground && !backTransparent)
								{
									sprite0hit = true;
								}
						//	}
						}

						
						
					}

					/*if(y>=0 && y < 256 && x>=0 && y <240)
					{
						backBuffer[y * 256 + x] = 0x00ff0000;
					}*/
				}

			}
		}

	}

}
void PPU::RenderBackground()
{
	uint16 realScan = (scanIndex);
	uint16 scanId = realScan + scrollH;

	//at each scanline, screen consists of 2 ,background screens (name tables) - left and right
	//0 == left nameTable, 1 == right nametable
	uint8 startColumn;
	uint8 endColumn;

	/*
	+-----------+-----------+  The PPU has enough VRAM
	|           |           |  for only two nametables;
	|           |           |  hardware on the cartridge
	|   $2000   |   $2400   |  maps $2800 to $2000
	|           |           |  and $2C00 to $2400
	|           |           |  (V-mirroring; Super Mario
	+-----------+-----------+  Bros.) or $2400 to $2000
	|           |           |  and $2C00 to $2800
	|           |           |  (H-mirroring; Legend of Zelda).
	|   $2800   |   $2C00   |
	|           |           |  See NES memory map
	|           |           |  for more information.
	+-----------+-----------+
	*/

	for(int i = 0; i < 2; i++)
	{
		uint16 baseTbl = nameTableAddress;
		if(i == 0) //left side, no need to mirror it horizontally!
		{
			//if need to scroll to new nametable vertically!
			if(scanId >= 240)
			{
				switch(baseTbl) //just flip vertically! : )
				{
				case 0x2000: baseTbl = 0x2800; break;
				case 0x2800: baseTbl = 0x2000; break;
				case 0x2400: baseTbl = 0x2C00; break;
				case 0x2C00: baseTbl = 0x2400; break;
				}
				scanId -= 240;
			}
			//start/end columns inside nametables! NOT screen columns!
			startColumn = scrollV / 8;
			endColumn = 32;

		}
		else //definitely need to swap horizontally! 
		{
			//if need to swap vertically
			if(scanId >= 240)
			{
				switch(baseTbl) //just flip vertically! : )
				{
				case 0x2000: baseTbl = 0x2C00; break;
				case 0x2400: baseTbl = 0x2800; break;
				case 0x2800: baseTbl = 0x2400; break;
				case 0x2C00: baseTbl = 0x2000; break;
				}
				scanId -= 240;
			}
			else
			{
				switch(baseTbl) //flip horizontally
				{
				case 0x2000: baseTbl = 0x2400; break;
				case 0x2400: baseTbl = 0x2000; break;
				case 0x2800: baseTbl = 0x2C00; break;
				case 0x2C00: baseTbl = 0x2800; break;
				}
			}
			startColumn = 0;
			endColumn = (scrollV / 8) + 1;
		}


		//handle cartridge mirroring!
		if(pNes->pCart->mirror_mode == MIRROR_VERTICAL)
		{
			switch(baseTbl)
			{
			case 0x2800: baseTbl = 0x2000; break;
			case 0x2C00: baseTbl = 0x2400; break;
			}
		}
		else if(pNes->pCart->mirror_mode == MIRROR_HORIZONTAL)
		{
			switch (baseTbl)
			{
			case 0x2400: baseTbl = 0x2000; break;
			case 0x2800: baseTbl = 0x2400; break;
			case 0x2C00: baseTbl = 0x2400; break;
			}
		}


	

		uint16 rowNum = scanId / 8;
		

		//printf("\n\n\n\n\n\nName table sprite indexes!\n\n");

		/*for(int j = 0; j < 30 ; j ++)
			for(int i = 0; i < 32 ; i ++)
			{
				//if(i == 4)
				//{
				
					uint16 id = video_ram[baseTbl + j * 32 + i];

					if(i == 5 && j ==14)
					{
						printf("COOL Column:%i  Row:%i  Id:%02x\n", i, j, id);
					}
					if(id==0x5f)
					{
						printf("Column:%i  Row:%i  Id:%02x\n", i, j, id);
					}
					
				//}
				

				
			}
			*/


		for(int column = startColumn; column < endColumn; column++)
		{
			//read tile number!
			//NameTable is array of 32*30 grid of tiles
			//1 tile == 1 byte == id in pattern table
			uint16 tileId = video_ram[baseTbl + rowNum * 32 + column];

			if(tileId < 0)
			{
				printf("INVALID TILE ID!!!\n");
			}
			uint16 patternOffset = backgroundPatternTbl + tileId * 16;

			uint8 tilePlane1 = pNes->pCart->ReadChrRom(patternOffset + (scanId % 8));
			uint8 tilePlane2 = pNes->pCart->ReadChrRom(patternOffset + (scanId % 8) + 8);

			//1 byte of attribute table == 4x4 tiles
			//32/4 = 8 columns
			uint8 blockAttributes = video_ram[(baseTbl + 0x3C0) + ((rowNum / 4) * 8) + column / 4];
			
			//4x4 tiles area is divided then to 4 2x2 bytes areas  (each 2 bytes = 2x2 tiles)
			blockAttributes >>= (((rowNum) % 4) / 2) * 4 + ((column % 4) / 2) * 2;

			blockAttributes = (blockAttributes & 0x3) << 2;



			uint8 startPixel;
			uint8 endPixel;

			if(i == 0) //left screen
			{
				//left edge of screen, maybe not all pixels to render
				if(column == startColumn)
					startPixel = scrollV % 8;
				else
					startPixel = 0;
				endPixel = 8;
			}
			else //right screen
			{
				if(column == endColumn)
					endPixel = scrollV % 8;
				else
					endPixel = 8;
				startPixel = 0;
			}

			for(int px = startPixel; px < endPixel; px++)
			{
				uint16 paletteOffset = blockAttributes | 
					(((tilePlane2 & (0x1 << (7 - px))) >> (7 - px)) << 1) |
					((tilePlane1 & (0x1 << (7 - px))) >> (7 - px));

				//check if not transparent pixel
				if(paletteOffset % 4)
				{
					//if rendering left side of screen
					if(i == 0)
					{
						backBufferSrcBack[realScan * 256 + 8*column - scrollV + px] = 
							colorPalette[(0x3f & video_ram[0x3F00 + paletteOffset])];
					}
					else //right
					{
						uint16 hOffset = 8*column + (256 - scrollV) + px;

						if(hOffset < 256)
						{
							backBufferSrcBack[realScan * 256 + hOffset] = 
								colorPalette[(0x3f & video_ram[0x3F00 + paletteOffset])];
						}
						
					}
				}
			}

		}
		
		
	}



}
void PPU::RenderBuffers()
{
		uint8 bgId = video_ram[0x3f00];
		bgId = bgId > 63 ? 0x0F : bgId;
		uint16 bgColor = colorPalette[bgId];


			//need to convert buffers
			for(int y = 0; y < 240; y++)
			{
				for(int x = 0; x < 256; x++)
				{
					int offset = y * 256 + x;

					uint16 src1 = backBufferSrcBack[offset];
					uint16 src2 = backBufferSrcSprite[offset];
					uint16 src;

					if(src2 !=0)
					{
						src = src2;
					}
					else
					{
						if(src1 !=0)
						{
							src = src1;
						}
						else
						{
							src = bgColor;
						}
					}

				/*	if(src == 0)
					{
						src = bgColor;
					}*/


					uint32 blue = (src & 0x1F) << 3;
					uint32 green = (src & 0x7E0) >> 3;
					uint32 red = (src & 0xF800) >> 8;

					uint32 color = (red << 16) | (green << 8) | blue;
					//backBuffer[offset] = color != 0 ? 0x00ff0000 : 0;
					//backBuffer[offset] = color;

					uint16 col = x * 2;
					uint16 row = y * 2;

					for(int i = 0; i < 4; i++)
					{
						backBuffer[(i >> 1) * 512 + i % 2 + row * 512 + col] = color;
					}
				}
			}
			Present();

			memset(backBuffer, 0, screenWidth * screenHeight * 4);
			memset(backBufferSrcBack, 0, 256*240*2);
			memset(backBufferSrcSprite, 0, 256*240 * 2);
}
void PPU::SetVblank(bool state)
{
	if(state)//V-Blank enable
	{
		inVBlank = state;
	}
	else
	{
		sprite0hit = false;

		inVBlank = state;
	}
	
}
bool PPU::IsNmiEnabled()
{
	return vblank_nmi;
}
bool PPU::GetVblank()
{
	return inVBlank;
}
void PPU::RenderScanline(uint16 scanId)
{
	scanIndex = scanId;

	if(scanIndex < 240) //only 240 scan lines to render!!!
	{
		scanHit = 0;

		if(!hideBackground)
			RenderBackground();

		if(!hideSprites)
			RenderSprites(false);
		
		if(!hideSprites)
			RenderSprites(true);

		if(scanHit >=8)
			spriteOverflow = true;
		else
			spriteOverflow = false;
	}



	/*if(scanIndex == ScanPerFrame)
	{
		


		scanIndex = 0;

		inVBlank = false;
		scanIndex = 0;
		writeEnable = true;
		sprite0hit = false;
	}
	if(scanIndex == 240)
	{
		if(vblank_nmi) 
			return true;
		else
			return false;
	}
	else if(scanIndex == ScanPerNMI)
	{
		RenderBuffers();
		char text[16];
		itoa(frameCounter, text, 10);
		pNes->pWindow->SetCaption(String(text));
		frameCounter++;
		inVBlank = true;

	
	}
	else
	{
		return false;
	}

	scanIndex++;
	return false;*/
}