#include "Cartridge.h"
#include "Error.h"

#include "Mapper0.h"
#include "Mapper1.h"
#include "Mapper2.h"
#include <Windows.h>

Cartridge::Cartridge()
{

}
Cartridge::~Cartridge()
{

}
uint8 Cartridge::ReadPrgRom(uint16 addr)
{
	uint16 baseAddress = addr - 0x8000;
	uint8 id = baseAddress / 0x1000;
	uint16 offset = baseAddress % 0x1000;

/*#ifdef _DEBUG
	if(id >= prg_page_count)
	{
		printf("Invalid PRG-ROM bank read!\n");
		Sleep(5000);
		exit(0);
	}
#endif*/
	return current_prg_rom_pages[id][offset];
}
int Cartridge::Load(const String& path)
{
	//read whole ROM to memory
	FILE* file = fopen(path.c_str(), "rb");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	pMemory = new uint8[size];
	int cntRead = fread(pMemory, 1, size, file);
	fclose(file);

	uint8* pData = pMemory;

	//parse ROM header
	if(pData[0] != 'N' || pData[1] != 'E' || pData[2] != 'S' || pData[3] != 0x1A)
	{
		return SC_FAIL;
	}

	prg_rom_num = pData[4];
	chr_rom_num = pData[5];

	


	uint8 controlByte = pData[6];
	uint8 controlByte2 = pData[7];

	mirror_mode = ((controlByte & 0x1) == 0) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL;
	mirror_mode = ((controlByte & 0x8) == 8) ? MIRROR_FOURSCREEN : mirror_mode;

	hasSaveRAM = (controlByte & 2) == 2;
	hasTrainer = (controlByte & 4) == 4;

//	controlByte >>= 4;

/*	if(controlByte2 & 0xf)
	{
		printf("Strange! Lower 4 bits on second control value must be NULL! But they aren't! o_O");
		controlByte2 = controlByte2 & 0xf0;
	}*/
	
//	mapper_num = controlByte | controlByte2;

	if(controlByte2 == 0x44)
	{
		mapper_num = controlByte >> 4;
	}
	else
	{
		mapper_num = (controlByte >> 4) + (controlByte2 & 0xf0);
	}


	ram_banks_num = pData[8];
	if(ram_banks_num == 0)
		ram_banks_num = 1;

	//move pointer to data location
	pData += 16;

	//if trainer presents - set pointer
	if(hasTrainer)
	{
		pTrainer = pData;
		pData += 512;
	}

	//set PRG-ROM pointer
	//prg_rom = pData;
	prg_page_count = prg_rom_num * 4;


	prg_rom = new uint8[prg_page_count * 4096];
	//memcpy(prg_rom, pData, 0x4000);
//	memcpy(prg_rom + 0x4000, pData + (0x4000 * (prg_rom_num - 1)), 0x4000);


	
	

	//setup prg rom memory pages pointers
	for(int i = 0; i < prg_page_count; i++)
	{
		prg_rom_pages[i] = prg_rom + (4096 * i);
		current_prg_rom_pages[i] = prg_rom_pages[i];
		memcpy(prg_rom_pages[i], pData + i * 4096, 4096);
	}


	//move to CHR_ROM;
	if(chr_rom_num)
	{
		pData += prg_rom_num * 0x4000; //move by 16 KB * prg_rom_num bytes
		chr_rom = pData;
		hasVRAM = false;
		chr_page_count = 8 * chr_rom_num;
	}
	else
	{
		chr_rom = new uint8[1024 * prg_page_count * 16];
		memset(chr_rom, 0, 1024 * prg_page_count * 16);
		chr_page_count = prg_page_count * 16;
		hasVRAM = true;
		chr_rom_num = 2;
	}


	//setup CHR ROM memory pages pointers
	for(int i = 0; i < chr_page_count; i++)
	{
		chr_rom_pages[i] = chr_rom + (1024 * i);
		current_chr_rom_pages[i] = chr_rom_pages[i];
	}
	
	

	//create RAM banks
	pRamBanks = new uint8[ram_banks_num * 0x2000];
	memset(pRamBanks, 0xFFFFFFFF, ram_banks_num * 0x2000);


	//create save RAM
	if(hasSaveRAM)
		pSaveRAM = new uint8[0x2000]; //8 KB save memory


	//setup memory pointers

	//setup mapper
	switch(mapper_num)
	{
	case 0:
		mapper = new Mapper0(this);
		break;
	case 1:
		mapper = new Mapper1(this);
		break;
	case 2:
		mapper = new Mapper2(this);
		break;
	default:
		printf("Sorry! Unsupported mapper #%u\nExit after 5 sec.", mapper_num);
		Sleep(5000);
		exit(0);
		break;
	}

	if (prg_rom_num == 1)
	{
		Mapper_SwitchPrg16(0, 1);
		Mapper_SwitchChr8(0);
	}


	mapper->Init();

	return SC_OK;
}

uint8 Cartridge::ReadChrRom(uint16 addr)
{
	uint16 baseAddress = addr;
	uint8 id = baseAddress / 0x400;
	uint16 offset = baseAddress % 0x400;

/*#ifdef _DEBUG
	if(id >= chr_page_count)
	{
		printf("Invalid CHR-ROM bank read!\n");
		Sleep(5000);
		exit(0);
	}
#endif*/
	return current_chr_rom_pages[id][offset];
}

void Cartridge::WritePrgRom(uint16 address, uint8 data)
{
	mapper->Write(address, data);
}