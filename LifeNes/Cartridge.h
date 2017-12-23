#ifndef Cartridge_h__
#define Cartridge_h__

#include "LString.h"
#include "Globals.h"
#include "IMapper.h"

class Cartridge
{
public:
	Cartridge();
	~Cartridge();


	void Mapper_SwitchPrg32(int start)
	{
		switch(prg_rom_num)
		{
		case 2:start = start & 0x7; break;
		case 4:start = start & 0xf; break;
		case 8:start = start & 0x1f; break;
		case 16:start = start & 0x3f; break;
		case 32:start = start & 0x7f; break;
		case 64:start = start & 0xff; break;
		case 128:start = start & 0x1ff; break;
		}

		for(int i = 0; i < 4; i++)
			current_prg_rom_pages[i] = prg_rom_pages[start + i];
	}

	void Mapper_SwitchPrg16(int start, int area)
	{
		switch(prg_rom_num)
		{
		case 2:start = start & 0x7; break;
		case 4:start = start & 0xf; break;
		case 8:start = start & 0x1f; break;
		case 16:start = start & 0x3f; break;
		case 32:start = start & 0x7f; break;
		case 64:start = start & 0xff; break;
		case 128:start = start & 0x1ff; break;
		}

		for(int i = 0; i < 4; i++)
			current_prg_rom_pages[4*area + i] = prg_rom_pages[start + i];
	}

	void Mapper_SwitchPrg8(int start, int area)
	{
		switch(prg_rom_num)
		{
		case 2:start = start & 0x7; break;
		case 4:start = start & 0xf; break;
		case 8:start = start & 0x1f; break;
		case 16:start = start & 0x3f; break;
		case 32:start = start & 0x7f; break;
		case 64:start = start & 0xff; break;
		case 128:start = start & 0x1ff; break;
		}

		for(int i = 0; i < 4; i++)
			current_prg_rom_pages[2 * area + i] = prg_rom_pages[start + i];
	}

	void Mapper_SwitchChr8(int start)
	{
		switch(chr_rom_num)
		{
		case 2:start = start & 0x7; break;
		case 4:start = start & 0x1f; break;
		case 8:start = start & 0x3f; break;
		case 16:start = start & 0x7f; break;
		case 32:start = start & 0xff; break;
		case 64:start = start & 0x1ff; break;
		}

		for(int i = 0; i < 8; i++)
			current_chr_rom_pages[i] = chr_rom_pages[start + i];
	}

	void Mapper_SwitchChr4(int start, int area)
	{
		switch(chr_rom_num)
		{
		case 2:start = start & 0xf; break;
		case 4:start = start & 0x1f; break;
		case 8:start = start & 0x3f; break;
		case 16:start = start & 0x7f; break;
		case 32:start = start & 0xff; break;
		case 64:start = start & 0x1ff; break;
		}

		for(int i = 0; i < 4; i++)
			current_chr_rom_pages[4 * area + i] = chr_rom_pages[start + i];
	}

	void Mapper_SwitchChr2(int start, int area)
	{
		switch(chr_rom_num)
		{
		case 2:start = start & 0xf; break;
		case 4:start = start & 0x1f; break;
		case 8:start = start & 0x3f; break;
		case 16:start = start & 0x7f; break;
		case 32:start = start & 0xff; break;
		case 64:start = start & 0x1ff; break;
		}

		for(int i = 0; i < 2; i++)
			current_chr_rom_pages[2 * area + i] = chr_rom_pages[start + i];
	}

	void Mapper_SwitchChr1(int start, int area)
	{
		switch(chr_rom_num)
		{
		case 2:start = start & 0xf; break;
		case 4:start = start & 0x1f; break;
		case 8:start = start & 0x3f; break;
		case 16:start = start & 0x7f; break;
		case 32:start = start & 0xff; break;
		case 64:start = start & 0x1ff; break;
		}

		current_chr_rom_pages[area] = chr_rom_pages[start];
	}



	int Load(const String& path);
	uint8* pMemory;
	
	uint8* chr_rom; //each bank = 8 kb
	uint8* pRamBanks;
	uint8* pTrainer;
	uint8* pSaveRAM;

	uint8* prg_rom_pages[256];
	uint8* chr_rom_pages[1024];

	uint8* current_prg_rom_pages[256];
	uint8* current_chr_rom_pages[1024];


	uint32 prg_page_count;
	uint32 chr_page_count;

	uint32 prg_rom_num;
	uint32 chr_rom_num;

	uint32 region;
	uint32 mirror_mode;
	uint32 mapper_num;
	uint32 ram_banks_num;

	bool hasSaveRAM;
	bool hasTrainer;
	bool hasVRAM;
	uint8 ReadChrRom(uint16 addr);
	uint8 ReadPrgRom(uint16 addr);

	void WritePrgRom(uint16 address, uint8 data);

	IMapper* mapper;
private:
	uint8* prg_rom; //each bank = 16 kb
	
};
#endif // Cartridge_h__
