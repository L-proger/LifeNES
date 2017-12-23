#include "NES.h"
#include "Error.h"
#include <Windows.h>

NES::NES(IRenderWindow* graphicsOut)
	:cartInserted(false), running(false), region(PAL), cyclesPerScanline(106), canReset(true),
	cycles_real(0), cycles_ideal(0.0f), cycles_dma(0), cycles_total(0)
{
	pWindow = graphicsOut;
	pCpu = new CPU(this);
	pPpu = new PPU(this);
	pApu = new APU(this);
	pJoy2 = new Joypad(this);
	pJoy1 = new Joypad(this);
	timer.Start();
	timer.Stop();
	
}
NES::~NES()
{

}
void NES::Update()
{
	//handle RESET button
	if(::GetKeyState(VK_F1) < 0)
	{
		if(canReset) Reset();
		canReset = false;
	}
	else canReset = true;

	//reduce "done cycles" count (to not overflow it after few hours of gameplay : ))
	uint32 reduceAmt = (uint32)floorf(cycles_ideal);
	if(reduceAmt > cycles_real) 
		reduceAmt = cycles_real;
	cycles_real -= reduceAmt;
	cycles_ideal -= (float)reduceAmt;

	float diff;
	float scan_time = 113.666666666666f;

	pPpu->BeginFrame();

	//render first 240 scanlines!
	for(int i = 0; i < 240; i++)
	{
		diff = cycles_ideal - (float)cycles_real;
		cycles_real += pCpu->DoCycles(scan_time + diff);
		cycles_ideal += scan_time;
		pPpu->RenderScanline(i);
	}

	//compose sprite + background planes and present on screen
	pPpu->RenderBuffers();

	//render 22 scan lines for V-Blank period
	for(int i = 240; i < 262; i++)
	{
		if(i == 241)
		{
			pPpu->SetVblank(true);

			//do 1 instruction before NMI
			diff = cycles_ideal - (float)cycles_real;
			cycles_real += pCpu->DoCycles(1.0f);
			cycles_ideal += 1.0f;

			if(pPpu->CanNMI())
			{
				pCpu->pendingIRQ |= NMI;
				pPpu->OnNMI();
			}

			//do all other instructions
			diff = cycles_ideal - (float)cycles_real;
			cycles_real += pCpu->DoCycles((scan_time - 1.0f) + diff);
			cycles_ideal += (scan_time - 1.0f);

		}
		else if(i == 261)
		{
			pApu->Render(pCpu->GetCycles());
			float cycles_ideal_frame = scan_time * 261.0f;
			diff = cycles_ideal_frame - (float)cycles_real;

			if(pPpu->CanNMI())
			{
				pCpu->pendingIRQ |= NMI;
				pPpu->OnNMI();
			}
			pPpu->SetVblank(false);
			cycles_ideal = scan_time + diff;
			cycles_real = pCpu->DoCycles(scan_time + diff);
		}
		else
		{
			if(pPpu->CanNMI())
			{
				pCpu->pendingIRQ |= NMI;
				pPpu->OnNMI();
			}

			diff = cycles_ideal - (float)cycles_real;
			cycles_real += pCpu->DoCycles(scan_time + diff);
			cycles_ideal += scan_time;
		}

	}

	//slow down to PAL/NTSC FPS  : )
	timer.Stop();
	while(true)
	{
		if(timer.getTimeElapsed() >= frameTime)
			break;
		timer.Stop();
	}
	timer.Start();
}
void NES::SetRegion(int _region)
{
	region = _region;
	if(region == NTSC)
	{
		cyclesPerScanline = 113;
		pPpu->ScanPerFrame = 261;
		frameTime = 1.0f / 60.0f;
		pPpu->ScanPerNMI = 241;
	}
	else
	{
		cyclesPerScanline = 106;
		pPpu->ScanPerFrame = 312;
		frameTime = 1.0f / 50.0f;
		pPpu->ScanPerNMI = 289;
	}

	
}
int NES::InsertCartridge(Cartridge* cart)
{
	if(!cart) return SC_FAIL;

	pCart = cart;
	cartInserted = true;

	//get pointer to the end of stack memory
	//cuz stack is REVERSED o_____O
	//stack memory is 256 byte (0x100-0x1FF)
//	pCpu->reg_sp = 0xff;
 }
void NES::EjectCartridge()
{
	pCart = 0;
	cartInserted = false;
}
void NES::Reset()
{
	printf("***************************\nSOFT RESET!\n***************************\n");
	//pCpu->reg_a = pCpu->reg_x = pCpu->reg_y = 0;
	//memset(pCart->pRamBanks,0, 0x2000);

	if(!running)
	{
		pCpu->reg_a = pCpu->reg_x = pCpu->reg_y = 0x00;
		pCpu->reg_sp = 0xFD;
		pCpu->reg_p.FromByte(0x34);
	}
	else
	{
		pCpu->reg_sp -= 3;
	}
	pCpu->reg_p.SetInterruptDisableFlag(true);
	cycles_dma = 0;
	pCpu->cycle_local = 0;
	pCpu->cycle_total = 0;
	cycles_ideal = 0.0f;
	cycles_real = 0;
	cycles_total = 0;

	pCpu->reg_pc = ReadMemory16(0xfffc); //read RESET interrupt handler pointer
}
void NES::Run()
{
	Reset();
	running = true;
}
void NES::Pause()
{

}
uint16 NES::ReadMemory16(uint16 address)
{
	if(address < 0x2000) //scratch RAM  (max = 3 banks of 8KB)
	{
		return reinterpret_cast<uint16*>(pCart->pRamBanks + address)[0];
	}
	else if(address < 0x8000) //save RAM
	{
		const int save_addr = address - 0x6000;
		return reinterpret_cast<uint16*>(pCart->pSaveRAM + save_addr)[0];
	}
	else //PRG_ROM
	{
		//const int prg_addr = address - 0x8000;

		uint8 data1 = pCart->ReadPrgRom(address);
		uint8 data2 = pCart->ReadPrgRom(address + 1);
		return (uint16(data2) << 8) + data1;
	}
}
uint8 NES::ReadMemory8(uint16 address)
{
	if(address < 0x2000) //scratch RAM  (max = 3 banks of 8KB)
	{
		address = address % 0x800;
		return pCart->pRamBanks[address];
	}
	else if(address < 0x4000) //MM IO
	{
		address = address % 0x8 + 0x2000; //address mirroring
		switch(address)
		{
		case 0x2002:
			{
				return pPpu->ReadStatusRegister();
			}
			break;
		case 0x2004:
			{
				return pPpu->ReadSpriteRAM();
			}
			break;
		case 0x2007:
			{
				return pPpu->ReadVRAM();
			}
			break;
		default:
			{
				printf("READ MEMORY 8  FOR IO IS NOT IMPLEMENTED!!!!\n");
				return 0x00;
			}
			break;
		}
		
		return 0x00;
	}
	else if(address < 0x4020) //MM IO
	{
		switch(address)
		{
		case 0x4015: //APU SOUND /VERTICAL clock signal
			{
				return pApu->ReadStatus();
			}
			break;
		case 0x4016:
			{
				return pJoy1->ReadState();
			}
			break;
		case 0x4017:
			{
				return pJoy2->ReadState();
			}
			break;
		default:
			{
				printf("READ MEMORY 8  FOR IO IS NOT IMPLEMENTED!!!!\n");
				return 0x00;
			}
		}
	}
	else if(address < 0x8000) //save RAM
	{
	//	const int save_addr = address - 0x6000;
//		return pCart->pSaveRAM[save_addr];
	}
	else //PRG_ROM
	{
		const int prg_addr = address - 0x8000;
		return pCart->ReadPrgRom(address);
		//return pCart->prg_rom[prg_addr];
	}
	return 0x00;
}
//not usable
void NES::WriteMemory16(uint16 address, uint16 val)
{
	if(address < 0x6000) //scratch RAM 
	{
		reinterpret_cast<uint16*>(pCart->pRamBanks + address)[0] = val;
	}
	else if(address < 0x8000) //save RAM
	{
		const int save_addr = address - 0x6000;
		reinterpret_cast<uint16*>(pCart->pSaveRAM + save_addr)[0] = val;
	}
	else //PRG_ROM
	{
		const int prg_addr = address - 0x8000;
		//reinterpret_cast<uint16*>(pCart->prg_rom + prg_addr)[0] = val;
	}
}

void NES::WriteMemory8(uint16 address, uint8 val)
{
	if(address < 0x2000) //scratch RAM 
	{
		address = address % 0x800;
		pCart->pRamBanks[address] = val;
	}
	else if(address < 0x4000) //IO REGISTERS
	{
		address = address % 0x8 + 0x2000; //address mirroring
		switch(address)
		{
		case 0x2000:
			pPpu->WriteControlRegister1(val);
			break;
		case 0x2001:
			pPpu->WriteControlRegister2(val);
			break;
		case 0x2003:
			pPpu->WriteSpriteRamAddress(val);
			break;
		case 0x2004:
			pPpu->WriteSpriteRAM(val);
			break;
		case 0x2005:
			pPpu->WriteVramAddress1(val);
			break;
		case 0x2006:
			pPpu->WriteVramAddress2(val);
			break;
		case 0x2007:
			pPpu->WriteVRAM(val);
			break;
		default:
		//	printf("IO REGISTER WRITE 8 IS NOT IMPLEMNTED FOR 0x%p!\n", address);
		//	(300);
			break;
		}
		
	}
	else if(address < 0x4020)
	{
		switch(address)
		{
			//Pulse channel 1
		case 0x4000: pApu->WriteChannelReg(0, 0, val); break;
		case 0x4001: pApu->WriteChannelReg(0, 1, val); break;
		case 0x4002: pApu->WriteChannelReg(0, 2, val); break;
		case 0x4003: pApu->WriteChannelReg(0, 3, val); break;

			//Pulse channel 2
		case 0x4004: pApu->WriteChannelReg(1, 0, val); break;
		case 0x4005: pApu->WriteChannelReg(1, 1, val); break;
		case 0x4006: pApu->WriteChannelReg(1, 2, val); break;
		case 0x4007: pApu->WriteChannelReg(1, 3, val); break;

			//Triangle channel
		case 0x4008: pApu->WriteChannelReg(2, 0, val); break;
		case 0x4009: pApu->WriteChannelReg(2, 1, val); break;
		case 0x400A: pApu->WriteChannelReg(2, 2, val); break;
		case 0x400B: pApu->WriteChannelReg(2, 3, val); break;

			//Noise channel
		case 0x400C: pApu->WriteChannelReg(3, 0, val); break;
		case 0x400E: pApu->WriteChannelReg(3, 1, val); break;
		case 0x400F: pApu->WriteChannelReg(3, 2, val); break;

			//DMC channel
		case 0x4010: pApu->WriteChannelReg(4, 0, val); break;
		case 0x4011: pApu->WriteChannelReg(4, 1, val); break;
		case 0x4012: pApu->WriteChannelReg(4, 2, val); break;
		case 0x4013: pApu->WriteChannelReg(4, 3, val); break;


		case 0x4014:
			pPpu->WriteSpriteDMA(val);
			cycles_dma += 514;
			break;
		case 0x4015:
			pApu->WriteStatus(val);
			break;
		case 0x4016:
			pJoy1->WriteState(val);
			//pJoy2->WriteState(val);
			break;
		case 0x4017:
			pApu->WriteConfigRegister(val);
			break;
		default:
		//	printf("IO REGISTER WRITE 8 IS NOT IMPLEMNTED FOR 0x%p!\n", address);
			//	(300);
			break;
		}
	}
	else if(address < 0x8000) //save RAM
	{
		//printf(" 3 sec! SAVE-RAM WRITE 8 IS NOT IMPLEMNTED FOR 0x%p!\n", address);
		//(3000);
	//	pCart->pSaveRAM[address - 0x6000] = val;
	}
	else
	{
		pCart->WritePrgRom(address, val);
	}
}
