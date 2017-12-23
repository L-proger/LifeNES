#include "CPU.h"
#include "NES.h"
#include <Windows.h>



//maybe need to add stack overflow check : 
CPU::CPU(NES* parent)
	:cycle_total(0), cycle_local(0), reg_sp(0xFD),
	reg_a(0), reg_x(0x0), reg_y(0), internalCycle(0), pendingIRQ(0)
{

	//pFile = fopen("C:\\LifeNes.txt", "w");
	stringBuffer = new char[1024];
	reg_p.FromByte(0x34);

	for(int i = 0; i <256; i++)
	{
		opcodes[i] = 0;
	}
	pNes = parent;
	//setup register pointers array
	//ADC
	opcodes[0x69] = &CPU::OpADC;
	opcodes[0x65] = &CPU::OpADC;
	opcodes[0x75] = &CPU::OpADC;
	opcodes[0x6D] = &CPU::OpADC;
	opcodes[0x7D] = &CPU::OpADC;
	opcodes[0x79] = &CPU::OpADC;
	opcodes[0x61] = &CPU::OpADC;
	opcodes[0x71] = &CPU::OpADC;
	//AND
	opcodes[0x29] = &CPU::OpAND;
	opcodes[0x25] = &CPU::OpAND;
	opcodes[0x35] = &CPU::OpAND;
	opcodes[0x2D] = &CPU::OpAND;
	opcodes[0x3D] = &CPU::OpAND;
	opcodes[0x39] = &CPU::OpAND;
	opcodes[0x21] = &CPU::OpAND;
	opcodes[0x31] = &CPU::OpAND;
	//ASL
	opcodes[0x0A] = &CPU::OpASL;
	opcodes[0x06] = &CPU::OpASL;
	opcodes[0x16] = &CPU::OpASL;
	opcodes[0x0E] = &CPU::OpASL;
	opcodes[0x1E] = &CPU::OpASL;
	//BCC
	opcodes[0x90] = &CPU::OpBCC;
	//BCS
	opcodes[0xB0] = &CPU::OpBCS;
	//BEQ
	opcodes[0xF0] = &CPU::OpBEQ;
	//BIT
	opcodes[0x24] = &CPU::OpBIT;
	opcodes[0x2C] = &CPU::OpBIT;
	//BMI
	opcodes[0x30] = &CPU::OpBMI;
	//BNE
	opcodes[0xD0] = &CPU::OpBNE;
	//BPL
	opcodes[0x10] = &CPU::OpBPL;
	//BRK
	opcodes[0x00] = &CPU::OpBRK;
	//BVC
	opcodes[0x50] = &CPU::OpBVC;
	//BVS
	opcodes[0x70] = &CPU::OpBVS;
	//CLC
	opcodes[0x18] = &CPU::OpCLC;
	//CLD
	opcodes[0xD8] = &CPU::OpCLD;
	//CLI
	opcodes[0x58] = &CPU::OpCLI;
	//CLV
	opcodes[0xB8] = &CPU::OpCLV;
	//CMP
	opcodes[0xC9] = &CPU::OpCMP;
	opcodes[0xC5] = &CPU::OpCMP;
	opcodes[0xD5] = &CPU::OpCMP;
	opcodes[0xCD] = &CPU::OpCMP;
	opcodes[0xDD] = &CPU::OpCMP;
	opcodes[0xD9] = &CPU::OpCMP;
	opcodes[0xC1] = &CPU::OpCMP;
	opcodes[0xD1] = &CPU::OpCMP;
	//CPX
	opcodes[0xE0] = &CPU::OpCPX;
	opcodes[0xE4] = &CPU::OpCPX;
	opcodes[0xEC] = &CPU::OpCPX;
	//CPY
	opcodes[0xC0] = &CPU::OpCPY;
	opcodes[0xC4] = &CPU::OpCPY;
	opcodes[0xCC] = &CPU::OpCPY;
	//DEC
	opcodes[0xC6] = &CPU::OpDEC;
	opcodes[0xD6] = &CPU::OpDEC;
	opcodes[0xCE] = &CPU::OpDEC;
	opcodes[0xDE] = &CPU::OpDEC;
	//DEX
	opcodes[0xCA] = &CPU::OpDEX;
	//DEY
	opcodes[0x88] = &CPU::OpDEY;
	//EOR
	opcodes[0x49] = &CPU::OpEOR;
	opcodes[0x45] = &CPU::OpEOR;
	opcodes[0x55] = &CPU::OpEOR;
	opcodes[0x4D] = &CPU::OpEOR;
	opcodes[0x5D] = &CPU::OpEOR;
	opcodes[0x59] = &CPU::OpEOR;
	opcodes[0x41] = &CPU::OpEOR;
	opcodes[0x51] = &CPU::OpEOR;
	//INC
	opcodes[0xE6] = &CPU::OpINC;
	opcodes[0xF6] = &CPU::OpINC;
	opcodes[0xEE] = &CPU::OpINC;
	opcodes[0xFE] = &CPU::OpINC;
	//INX
	opcodes[0xE8] = &CPU::OpINX;
	//INY
	opcodes[0xC8] = &CPU::OpINY;

	//ISB
	opcodes[0xFF] = &CPU::OpSBC; //illegal ISB
	opcodes[0xE3] = &CPU::OpSBC; //illegal ISB

	//JMP
	opcodes[0x4C] = &CPU::OpJMP;
	opcodes[0x6C] = &CPU::OpJMP;
	//JSR
	opcodes[0x20] = &CPU::OpJSR;

	//KIL
	opcodes[0x02] = &CPU::OpKIL; //All KIL are illegal
	opcodes[0x12] = &CPU::OpKIL;
	opcodes[0x22] = &CPU::OpKIL;
	opcodes[0x32] = &CPU::OpKIL;
	opcodes[0x42] = &CPU::OpKIL;
	opcodes[0x52] = &CPU::OpKIL;
	opcodes[0x62] = &CPU::OpKIL;
	opcodes[0x72] = &CPU::OpKIL;
	opcodes[0x92] = &CPU::OpKIL;
	opcodes[0xB2] = &CPU::OpKIL;
	opcodes[0xD2] = &CPU::OpKIL;
	opcodes[0xF2] = &CPU::OpKIL;



	//LAX
	opcodes[0xAF] = &CPU::OpLAX; //All LAX are illegal
	opcodes[0xBF] = &CPU::OpLAX;
	opcodes[0xA7] = &CPU::OpLAX;
	opcodes[0xB7] = &CPU::OpLAX;
	opcodes[0xA3] = &CPU::OpLAX;
	opcodes[0xB3] = &CPU::OpLAX;

	//LDA
	opcodes[0xA9] = &CPU::OpLDA;
	opcodes[0xA5] = &CPU::OpLDA;
	opcodes[0xB5] = &CPU::OpLDA;
	opcodes[0xAD] = &CPU::OpLDA;
	opcodes[0xBD] = &CPU::OpLDA;
	opcodes[0xB9] = &CPU::OpLDA;
	opcodes[0xA1] = &CPU::OpLDA;
	opcodes[0xB1] = &CPU::OpLDA;
	//LDX
	opcodes[0xA2] = &CPU::OpLDX;
	opcodes[0xA6] = &CPU::OpLDX;
	opcodes[0xB6] = &CPU::OpLDX;
	opcodes[0xAE] = &CPU::OpLDX;
	opcodes[0xBE] = &CPU::OpLDX;
	//LDY
	opcodes[0xA0] = &CPU::OpLDY;
	opcodes[0xA4] = &CPU::OpLDY;
	opcodes[0xB4] = &CPU::OpLDY;
	opcodes[0xAC] = &CPU::OpLDY;
	opcodes[0xBC] = &CPU::OpLDY;
	//LSR
	opcodes[0x4A] = &CPU::OpLSR;
	opcodes[0x46] = &CPU::OpLSR;
	opcodes[0x56] = &CPU::OpLSR;
	opcodes[0x4E] = &CPU::OpLSR;
	opcodes[0x5E] = &CPU::OpLSR;
	//NOP
	opcodes[0xEA] = &CPU::OpNOP;
	opcodes[0x80] = &CPU::OpNOP; //illegal
	opcodes[0x82] = &CPU::OpNOP; //illegal
	opcodes[0x1A] = &CPU::OpNOP; //illegal
	opcodes[0x3A] = &CPU::OpNOP; //illegal
	//ORA
	opcodes[0x09] = &CPU::OpORA;
	opcodes[0x05] = &CPU::OpORA;
	opcodes[0x15] = &CPU::OpORA;
	opcodes[0x0D] = &CPU::OpORA;
	opcodes[0x1D] = &CPU::OpORA;
	opcodes[0x19] = &CPU::OpORA;
	opcodes[0x01] = &CPU::OpORA;
	opcodes[0x11] = &CPU::OpORA;
	//PHA
	opcodes[0x48] = &CPU::OpPHA;
	//PHP
	opcodes[0x08] = &CPU::OpPHP;
	//PLA
	opcodes[0x68] = &CPU::OpPLA;
	//PLP
	opcodes[0x28] = &CPU::OpPLP;
	//ROL
	opcodes[0x2A] = &CPU::OpROL;
	opcodes[0x26] = &CPU::OpROL;
	opcodes[0x36] = &CPU::OpROL;
	opcodes[0x2E] = &CPU::OpROL;
	opcodes[0x3E] = &CPU::OpROL;
	//ROR
	opcodes[0x6A] = &CPU::OpROR;
	opcodes[0x66] = &CPU::OpROR;
	opcodes[0x76] = &CPU::OpROR;
	opcodes[0x6E] = &CPU::OpROR;
	opcodes[0x7E] = &CPU::OpROR;
	//RTI
	opcodes[0x40] = &CPU::OpRTI;
	//RTS
	opcodes[0x60] = &CPU::OpRTS;
	//SBC
	opcodes[0xE9] = &CPU::OpSBC;
	opcodes[0xE5] = &CPU::OpSBC;
	opcodes[0xF5] = &CPU::OpSBC;
	opcodes[0xEB] = &CPU::OpSBC; //illegal
	opcodes[0xED] = &CPU::OpSBC;
	opcodes[0xFD] = &CPU::OpSBC;
	opcodes[0xF9] = &CPU::OpSBC;
	opcodes[0xE1] = &CPU::OpSBC;
	opcodes[0xF1] = &CPU::OpSBC;
	//SEC
	opcodes[0x38] = &CPU::OpSEC;
	//SED
	opcodes[0xF8] = &CPU::OpSED;
	//SEI
	opcodes[0x78] = &CPU::OpSEI;
	//STA
	opcodes[0x85] = &CPU::OpSTA;
	opcodes[0x95] = &CPU::OpSTA;
	opcodes[0x8D] = &CPU::OpSTA;
	opcodes[0x9D] = &CPU::OpSTA;
	opcodes[0x99] = &CPU::OpSTA;
	opcodes[0x81] = &CPU::OpSTA;
	opcodes[0x91] = &CPU::OpSTA;
	//STX
	opcodes[0x86] = &CPU::OpSTX;
	opcodes[0x96] = &CPU::OpSTX;
	opcodes[0x8E] = &CPU::OpSTX;
	//STY
	opcodes[0x84] = &CPU::OpSTY;
	opcodes[0x94] = &CPU::OpSTY;
	opcodes[0x8C] = &CPU::OpSTY;
	//TAX
	opcodes[0xAA] = &CPU::OpTAX;
	//TAY
	opcodes[0xA8] = &CPU::OpTAY;
	//TSX
	opcodes[0xBA] = &CPU::OpTSX;
	//TXA
	opcodes[0x8A] = &CPU::OpTXA;
	//TXS
	opcodes[0x9A] = &CPU::OpTXS;
	//TYA
	opcodes[0x98] = &CPU::OpTYA;
}

CPU::~CPU()
{

}

int CPU::DoCycles(float count)
{
	int doneCycles = 0;
	while(count > 0)
	{
		if(pNes->cycles_dma > 0)
		{
		//	printf("DMA cycles burning\n");
			if(uint32(count) <= pNes->cycles_dma)
			{

				doneCycles += uint32(count);
				pNes->cycles_dma -= uint32(count);
				goto cycles_done;
			}
			else
			{
				count -= (float)pNes->cycles_dma;
				doneCycles += pNes->cycles_dma;
				pNes->cycles_dma = 0;
			}
		}
		
		int executed = Execute();
		doneCycles += executed;
		count = count - (float)executed;
	}
cycles_done:
	return doneCycles;
}

int CPU::Execute()
{
	cycle_local = 0;

	//store previous opcode
	opcode_previous = opcode_current;

	//NMI is pending
	if(pendingIRQ & NMI)
	{
		PushStack16(reg_pc);
		reg_p.SetBreakCommandFlag(false); //hmm
		PushStack8(reg_p.ToByte());
		reg_p.SetInterruptDisableFlag(true);
		reg_pc = pNes->ReadMemory16(0xFFFA);
		cycle_local +=7;

		//remove NMI flag
		pendingIRQ &= ~NMI;
	}
	//read next opcode
	opcode_current = pNes->ReadMemory8(reg_pc);
	if(opcodes[opcode_current] == 0)
	{
		printf("ERROR!!! Invalid opcode value: $%02x\n", opcode_current);
		Sleep(9000);
		exit(0);
	}
	//execute opcode
	(this->*opcodes[opcode_current])();

	cycle_total += cycle_local;
	return cycle_local;
}

void CPU::Update()
{
/*	//store previous opcode
	opcode_previous = opcode_current;
	//read next opcode
	opcode_current = pNes->ReadMemory8(reg_pc);

	//execute opcode
	(this->*opcodes[opcode_current])();

	//count cycles


	if(cycle_local >= pNes->cyclesPerScanline)
	{
		if(pNes->pPpu->RenderScanline())
		{
			PushStack16(reg_pc);
			PushStack8(reg_p.ToByte());
			reg_pc = pNes->ReadMemory16(0xFFFA);
			cycle_local +=7;
		}
		cycle_local -= pNes->cyclesPerScanline;
	}*/
}

//STACK OPERATIONS
void CPU::PushStack8(uint8 val)
{
	pNes->WriteMemory8(0x100 + reg_sp, val);
	reg_sp--;
}
uint8 CPU::PopStack8()
{
	reg_sp++;
	return pNes->ReadMemory8(0x100 + reg_sp);
}
void CPU::PushStack16(uint16 val)
{
	uint8 b1 = val >> 8;
	uint8 b2 = val & 0xff;
	PushStack8(b1);
	PushStack8(b2);
}
uint16 CPU::PopStack16()
{
	uint8 b1 = PopStack8();
	uint8 b2 = PopStack8();
	return MakeAddress16(b1, b2);
}

//MEMORY READ
uint8 CPU::ZeroPage(uint16 addr)
{
	return pNes->ReadMemory8(addr);
}
uint8 CPU::ZeroPageX(uint16 addr)
{
	return pNes->ReadMemory8((addr + reg_x) & 0xff);
}
uint8 CPU::ZeroPageY(uint16 addr)
{
	return pNes->ReadMemory8((addr + reg_y) & 0xff);
}
uint8 CPU::Absolute(uint8 b1, uint8 b2)
{
	uint16 a = MakeAddress16(b1, b2);
	return pNes->ReadMemory8(a);
}
uint8 CPU::AbsoluteX(uint8 b1, uint8 b2, bool checkBoundary)
{
	uint16 addr1 = MakeAddress16(b1, b2);
	uint16 addr2 = addr1 + reg_x;
	if(checkBoundary)
	{
		if((uint8)addr2 < reg_x)
			cycle_local++;
	}
	return pNes->ReadMemory8(addr2);
}
uint8 CPU::AbsoluteY(uint8 b1, uint8 b2, bool checkBoundary)
{
	uint16 addr1 = MakeAddress16(b1, b2);
	uint16 addr2 = addr1 + reg_y;
	if(checkBoundary)
	{
		//if(addr1 & 0xff00 != addr2 & 0xff00)
		if((uint8)addr2 < reg_y)
			cycle_local++;
	}
	//reg_pc +=2;
	return pNes->ReadMemory8(addr2);
}
uint8 CPU::IndirectX(uint8 b1)
{
	uint8 val1 = pNes->ReadMemory8((b1 + reg_x) & 0xFF);
	uint8 val2 = pNes->ReadMemory8((b1 + reg_x + 1) & 0xFF);

	uint16 r0 = MakeAddress16(val1, val2);
	return pNes->ReadMemory8(r0);
}
uint8 CPU::IndirectY(uint8 b1, bool checkBoundary)
{
	uint8 val1 = pNes->ReadMemory8(b1);
	uint8 val2 = pNes->ReadMemory8((b1 + 1) & 0xFF);

	uint32 address = MakeAddress16(val1, val2);

	if(checkBoundary)
	{
		
		//if((address & 0xff00) != ((address + reg_y) & 0xff00))
		if(uint8(address + reg_y) < reg_y)
		{
			cycle_local++;
		}
	}
	return pNes->ReadMemory8((address + reg_y) & 0xffff);
}

//MEMORY WRITE
void CPU::ZeroPageWrite(uint16 addr, uint8 data)
{
	pNes->WriteMemory8(addr, data);
}
void CPU::ZeroPageXWrite(uint16 addr, uint8 data)
{
	pNes->WriteMemory8((addr + reg_x) & 0xff, data);
}
void CPU::ZeroPageYWrite(uint16 addr, uint8 data)
{
	pNes->WriteMemory8((addr + reg_y) & 0xff, data);
}
void CPU::AbsoluteWrite(uint8 b1, uint8 b2, uint8 data)
{
	uint16 a = MakeAddress16(b1, b2);
	pNes->WriteMemory8(a, data);
}
void CPU::AbsoluteXWrite(uint8 b1, uint8 b2, uint8 data)
{
	uint16 addr1 = MakeAddress16(b1, b2);
	uint16 addr2 = addr1 + reg_x;
	pNes->WriteMemory8(addr2, data);
}
void CPU::AbsoluteYWrite(uint8 b1, uint8 b2, uint8 data)
{
	uint16 addr1 = MakeAddress16(b1, b2);
	uint16 addr2 = addr1 + reg_y;
	pNes->WriteMemory8(addr2, data);
}
void CPU::IndirectXWrite(uint8 b1, uint8 data)
{
	uint8 val1 = pNes->ReadMemory8((b1 + reg_x) & 0xFF);
	uint8 val2 = pNes->ReadMemory8((b1 + reg_x + 1) & 0xFF);

	uint16 r0 = MakeAddress16(val1, val2);
    pNes->WriteMemory8(r0, data);
}
void CPU::IndirectYWrite(uint8 b1, uint8 data)
{
	uint8 val1 = pNes->ReadMemory8(b1);
	uint8 val2 = pNes->ReadMemory8((b1 + 1) & 0xFF);

	uint32 address = MakeAddress16(val1, val2);

    pNes->WriteMemory8((address + reg_y) & 0xffff, data);
}

//OPCODES

//fwrite(stringBuffer,1, strlen(stringBuffer), pFile);
//ready+
void CPU::OpADC() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	uint8 tv = 0;

	switch(opcode_current)
	{
	case 0x69:
		tv = val1; reg_pc += 2; cycle_local += 2;
		break;
	case 0x65:
		tv = ZeroPage(val1); reg_pc += 2; cycle_local += 3;
		break;
	case 0x75:
		tv = ZeroPageX(val1); reg_pc += 2; cycle_local += 4;
		break;
	case 0x6D:
		tv = Absolute(val1, val2); reg_pc += 3; cycle_local += 4;
		break;
	case 0x7D:
		tv = AbsoluteX(val1, val2, true); reg_pc += 3; cycle_local += 4;
		break;
	case 0x79:
		tv = AbsoluteY(val1, val2, true); reg_pc += 3; cycle_local += 4;
		break;
	case 0x61:
		tv = IndirectX(val1); reg_pc += 2; cycle_local += 6;
		break;
	case 0x71:
		tv = IndirectY(val1, true); reg_pc += 2; cycle_local += 5;
		break;
	}

	uint32 val32 = uint32(reg_a) + uint32(tv);
	if(reg_p.GetCarryFlag())
		val32++;

	reg_p.SetCarryFlag(val32 > 0xFF);

	reg_p.SetOverflowFlag((((~(reg_a ^ tv)) & (reg_a ^ val32) & 0x80) == 0x80));

	reg_a = val32 & 0xff;

	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_a));
	
	


	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x gfgfgfgf  SP:%02x P:: n: v: u: b: d: i: z: c: \n", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp, reg_p.ToByte());
}
//ready+
void CPU::OpAND() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;
	switch(opcode_current)
	{
	case 0x29:
		{
			reg_a = reg_a & val1;
			reg_pc += 2;
			cycle_local += 2;
		}
		break;
	case 0x25:
		{
			val1 = ZeroPage(val1);
			reg_a = reg_a & val1;
			reg_pc += 2;
			cycle_local += 3;
		}
		break;
	case 0x35:
		{
			val1 = ZeroPageX(val1);
			reg_a = reg_a & val1;
			reg_pc += 2;
			cycle_local += 4;
		}
		break;
	case 0x2D:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			val1 = Absolute(val1, val2);
			reg_a = reg_a & val1;
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0x3D:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			val1 = AbsoluteX(val1, val2, true);
			reg_a = reg_a & val1;
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0x39:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			val1 = AbsoluteY(val1, val2, true);
			reg_a = reg_a & val1;
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0x21:
		{
			val1 = IndirectX(val1);
			reg_a = reg_a & val1;
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0x31:
		{
			val1 = IndirectY(val1, true);
			reg_a = reg_a & val1;
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	}

	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_a));



	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpASL() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	uint8 tv;

	switch(opcode_current)
	{
	case 0x0A:
		{
			tv = reg_a;
			reg_p.SetCarryFlag(tv & (0x1 << 7));
			tv <<= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_a = tv;
			reg_pc+=1;
			cycle_local+=2;
		}
		break;
	case 0x06:
		{
			tv = ZeroPage(val1);
			reg_p.SetCarryFlag(tv & (0x1 << 7));
			tv <<= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			ZeroPageWrite(val1, tv);
			reg_pc+=2;
			cycle_local+=5;
		}
		break;
	case 0x16:
		{
			tv = ZeroPageX(val1);
			reg_p.SetCarryFlag(tv & (0x1 << 7));
			tv <<= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			ZeroPageXWrite(val1, tv);
			reg_pc+=2;
			cycle_local+=6;
		}
		break;
	case 0x0E:
		{
			tv = Absolute(val1, val2);
			reg_p.SetCarryFlag(tv & (0x1 << 7));
			tv <<= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			AbsoluteWrite(val1, val2, tv);
			reg_pc+=3;
			cycle_local+=6;
		}
		break;
	case 0x1E:
		{
			tv = AbsoluteX(val1, val2, false);
			reg_p.SetCarryFlag(tv & (0x1 << 7));
			tv <<= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			AbsoluteXWrite(val1, val2, tv);
			reg_pc+=3;
			cycle_local+=7;
		}
		break;
	}

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBCC() 
{
	if(!reg_p.GetCarryFlag())
	{
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}

		reg_pc += val;
		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBCS() 
{
	if(reg_p.GetCarryFlag())
	{
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}

		reg_pc += val;
		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBEQ() 
{
	if(reg_p.GetZeroFlag())
	{
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}

		reg_pc += val;
		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;

	////printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);    
}
//ready+
void CPU::OpBIT() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 tv;

	switch(opcode_current)
	{
	case 0x24:
		tv = ZeroPage(val1); reg_pc += 2; cycle_local += 3;
		break;
	case 0x2C:
		uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
		tv = Absolute(val1, val2); reg_pc += 3; cycle_local += 4;
		break;
	}


	reg_p.SetZeroFlag((tv & reg_a) == 0);
	reg_p.SetOverflowFlag(tv & (0x1 << 6));
	reg_p.SetNegativeFlag(tv & (0x1 << 7));

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBMI() 
{
	if(reg_p.GetNegativeFlag())
	{
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}

		reg_pc += val;
		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+++++++++++
void CPU::OpBNE() 
{
	if(!reg_p.GetZeroFlag())
	{
		
		uint16 old = reg_pc;
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}

		reg_pc += val;

		//printf("$%04x  BNE[%02x] DST:%04x\n", old, opcode_current, reg_pc);

		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBPL() 
{
	if(!reg_p.GetNegativeFlag())
	{
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}
		
		reg_pc += val;
		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBRK() 
{
	/*reg_pc +=2;
	PushStack16(reg_pc);
	reg_p.SetBreakCommandFlag(true);
	PushStack8(reg_p.ToByte());
	reg_p.SetInterruptDisableFlag(true); //maybe not need cuz not documented!!!!
	reg_pc = pNes->ReadMemory16(0xFFFE);
	cycle_local +=7;
	*/
	PushStack16(reg_pc + 2);
	//reg_p.SetBreakCommandFlag(true);
	PushStack8(reg_p.ToByte() | 0x30);
	reg_p.SetInterruptDisableFlag(true);
	cycle_local +=(7);
	reg_pc = pNes->ReadMemory16(0xFFFE);

//	printf("WARNING! BRK opcode! Need to check it out!!!\n");
	
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBVC() 
{
	if(!reg_p.GetOverflowFlag())
	{
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}

		reg_pc += val;
		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpBVS() 
{
	if(reg_p.GetOverflowFlag())
	{
		sint8 val = pNes->ReadMemory8(reg_pc + 1);
		reg_pc += 2;

		if((reg_pc & 0xff00) != ((reg_pc + val) & 0xff00))
		{
			cycle_local++;
		}

		reg_pc += val;
		cycle_local++;
	}
	else
	{
		reg_pc+=2;
	}

	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpCLC() 
{
	reg_p.SetCarryFlag(false);
	reg_pc+=1;
	cycle_local+=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpCLD()
{
	reg_p.SetDecimalModeFlag(false);
	cycle_local+=2;
	reg_pc+=1;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpCLI() 
{
	reg_p.SetInterruptDisableFlag(false);
	reg_pc+=1;
	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpCLV() 
{
	reg_p.SetOverflowFlag(false);
	reg_pc += 1;
	cycle_local += 2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpCMP() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 operand;
	switch(opcode_current)
	{
	case 0xC9:
		{
			operand = val1;
			reg_pc += 2;
			cycle_local += 2;
		}
		break;
	case 0xC5:
		{
			operand = ZeroPage(val1);
			reg_pc += 2;
			cycle_local += 3;
		}
		break;
	case 0xD5:
		{
			operand = ZeroPageX(val1);
			reg_pc += 2;
			cycle_local += 4;
		}
		break;
	case 0xCD:
		{
			operand = pNes->ReadMemory8(reg_pc + 2);
			operand = Absolute(val1, operand);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0xDD:
		{
			operand = pNes->ReadMemory8(reg_pc + 2);
			operand = AbsoluteX(val1, operand, true);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0xD9:
		{
			operand = pNes->ReadMemory8(reg_pc + 2);
			operand = AbsoluteY(val1, operand, true);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0xC1:
		{
			operand = IndirectX(val1);
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0xD1:
		{
			operand = IndirectY(val1, true);
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	}

	reg_p.SetCarryFlag(reg_a >= operand);
	operand = reg_a - operand;
	reg_p.SetZeroFlag(operand == 0);
	
	reg_p.SetNegativeFlag(operand & (0x1 << 7));

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpCPX() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 tv;
	switch(opcode_current)
	{
	case 0xE0:
		{
			tv = val1;
			reg_pc += 2;
			cycle_local += 2;
		}
		break;
	case 0xE4:
		{
			tv = ZeroPage(val1);
			reg_pc += 2;
			cycle_local += 3;
		}
		break;
	case 0xEC:
		{
			uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
			tv = Absolute(val1, val2);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	}

	reg_p.SetCarryFlag(reg_x >= tv);
	tv = reg_x - tv;
	reg_p.SetZeroFlag(tv == 0);
	reg_p.SetNegativeFlag(tv & (0x1 << 7));

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpCPY() 
{
	uint8 val = pNes->ReadMemory8(reg_pc + 1);
	uint8 tv = 0;
	
	switch(opcode_current)
	{
	case 0xC0:
		{
			tv = val;
			reg_pc += 2;
			cycle_local += 2;
		}
		break;
	case 0xC4:
		{
			tv = ZeroPage(val);
			reg_pc += 2;
			cycle_local += 3;
		}
		break;
	case 0xCC:
		{
			uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
			tv = Absolute(val, val2);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	}

	reg_p.SetCarryFlag(reg_y >= tv);
	tv = reg_y - tv;
	reg_p.SetZeroFlag(tv == 0);
	reg_p.SetNegativeFlag(tv & (0x1 << 7));
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpDEC() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	uint8 tv;

	switch(opcode_current)
	{
	case 0xC6:
		{
			tv = ZeroPage(val1);
			tv--;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			ZeroPageWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	case 0xD6:
		{
			tv = ZeroPageX(val1);
			tv--;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			ZeroPageXWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0xCE:
		{
			tv = Absolute(val1, val2);
			tv--;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			AbsoluteWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 6;
		}
		break;
	case 0xDE:
		{
			tv = AbsoluteX(val1, val2, false);
			tv--;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			AbsoluteXWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 7;
		}
		break;
	}

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpDEX() 
{
	reg_x--;
	reg_p.SetZeroFlag(reg_x == 0);
	reg_p.SetNegativeFlag(reg_x & (0x1 << 7));
	reg_pc +=1;
	cycle_local+=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpDEY() 
{
	reg_y--;
	reg_p.SetZeroFlag(reg_y == 0);
	reg_p.SetNegativeFlag(reg_y & (0x1 << 7));

	reg_pc+=1;
	cycle_local +=2;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpEOR() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	uint8 tv;
	switch(opcode_current)
	{
	case 0x49:
		tv = val1; reg_pc += 2; cycle_local += 2;
		break;
	case 0x45:
		tv = ZeroPage(val1); reg_pc += 2; cycle_local += 3;
		break;
	case 0x55:
		tv = ZeroPageX(val1); reg_pc += 2; cycle_local += 4;
		break;
	case 0x4D:
		tv = Absolute(val1, val2); reg_pc += 3; cycle_local += 4;
		break;
	case 0x5D:
		tv = AbsoluteX(val1, val2, true); reg_pc += 3; cycle_local += 4;
		break;
	case 0x59:
		tv = AbsoluteY(val1, val2, true); reg_pc += 3; cycle_local += 4;
		break;
	case 0x41:
		tv = IndirectX(val1); reg_pc += 2; cycle_local += 6;
		break;
	case 0x51:
		tv = IndirectY(val1, true); reg_pc += 2; cycle_local += 5;
		break;
	}

	reg_a = reg_a ^ tv;
	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_a));

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpINC() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	uint8 tv;

	switch(opcode_current)
	{
	case 0xE6:
		{
			tv = ZeroPage(val1);
			tv++;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			ZeroPageWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	case 0xF6:
		{
			tv = ZeroPageX(val1);
			tv++;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			ZeroPageXWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0xEE:
		{
			tv = Absolute(val1, val2);
			tv++;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			AbsoluteWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 6;
		}
		break;
	case 0xFE:
		{
			tv = AbsoluteX(val1, val2, false);
			tv++;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(IS_NEG(tv));
			AbsoluteXWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 7;
		}
		break;
	}

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
	
}
//ready++++++++++
void CPU::OpINX() 
{
	reg_x++;
	reg_p.SetZeroFlag(reg_x == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_x));
	reg_pc +=1;
	cycle_local +=2;


	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpINY() 
{
	reg_y+=1;
	reg_p.SetZeroFlag(reg_y == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_y));
	reg_pc+=1;
	cycle_local +=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpJMP() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);

	if(opcode_current == 0x4C)
	{
		
		reg_pc = MakeAddress16(val1, val2);
		cycle_local += 3;
	}
	else
	{
		uint16 addr = MakeAddress16(val1, val2);

		if((addr & 0xff) == 0xff) //on page edge
		{
			uint8 p1 = pNes->ReadMemory8(addr);
			uint8 p2 = pNes->ReadMemory8(addr & 0xff00);

			reg_pc = MakeAddress16(p1, p2);
		}
		else
		{
			reg_pc = pNes->ReadMemory16(addr);
		}


		//reg_pc = pNes->ReadMemory16(val1);
	//	if((reg_pc & 0xff) == 0xff) 
	//	{
	//		reg_pc &= 0xff00;
		//	printf("[!] Info: JMP indirect address fix (fall on page boundary)\n");
	//	}
		cycle_local += 5;
	}

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpJSR() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	
	PushStack16(reg_pc + 2);
	reg_pc = MakeAddress16(val1, val2); //Absolute(val1, val2);
	cycle_local += 6;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}

void CPU::OpKIL()  //illegal
{
	printf("CPU HALT!\n");
	return;
}

void CPU::OpLAX()
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	switch(opcode_current)
	{
	case 0xAF: //Abs
		{

			reg_a = Absolute(val1, val2);
			reg_x = reg_a;
			cycle_local += 4;
			reg_pc += 3;

			/*reg_a = val1;
			//printf("[m] 0x%p | ", val1);
			cycle_local += 4;
			reg_pc += 2;*/
		}
		break;
	case 0xBF: //absY
		{
			reg_a = AbsoluteY(val1, val2, true);
			cycle_local += 4;
			reg_x = reg_a;
			reg_pc += 3;


			
		}
		break;
	case 0xA7: //ZP 3cyc
		{
			reg_a = ZeroPage(val1);
			reg_x = reg_a;
			cycle_local += 3;
			reg_pc += 2;

			/*reg_a = ZeroPageX(val1);

			cycle_local += 4;
			reg_pc += 2;*/
		}
		break;
	case 0xB7: //ZeroY
		{
			reg_a = ZeroPageY(val1);
			reg_x = reg_a;
			cycle_local += 4;
			reg_pc += 2;
		}
		break;
	case 0xA3: //IndX
		{
			reg_a = IndirectX(val1);
			reg_x = reg_a;
			reg_pc += 2;
			cycle_local += 6;

			/*val2 = pNes->ReadMemory8(reg_pc + 2);
			reg_a = AbsoluteX(val1, val2, true);
			cycle_local += 4;
			reg_pc += 3;*/
		}
		break;
	case 0xB3: //IndY
		{
			reg_a = IndirectY(val1, true);
			reg_x = reg_a;
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	/*case 0xA1:
		{
			
		}
		break;
	case 0xB1:
		{
			
		}
		break;*/
	}

	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(reg_a & (0x1 << 7));
}
//ready+
void CPU::OpLDA() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;
	switch(opcode_current)
	{
	case 0xA9:
		{
			reg_a = val1;
			//printf("[m] 0x%p | ", val1);
			cycle_local += 2;
			reg_pc += 2;
		}
		break;
	case 0xA5:
		{
			
			reg_a = ZeroPage(val1);
		//	printf("0x%p | ", val1);
			
			cycle_local += 3;
			reg_pc += 2;
		}
		break;
	case 0xB5:
		{
			reg_a = ZeroPageX(val1);
		
			cycle_local += 4;
			reg_pc += 2;
		}
		break;
	case 0xAD:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			uint16 add = MakeAddress16(val1, val2);
			if(add == 0x3ffa)
			{
				printf("Found");
			}
			reg_a = Absolute(val1, val2);
			
		//	printf("0x%p | ", MakeAddress16(val1, val2));
			cycle_local += 4;
			reg_pc += 3;
		}
		break;
	case 0xBD:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			reg_a = AbsoluteX(val1, val2, true);
			cycle_local += 4;
			reg_pc += 3;
		}
		break;
	case 0xB9:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			reg_a = AbsoluteY(val1, val2, true);
			cycle_local += 4;
			reg_pc += 3;
		}
		break;
	case 0xA1:
		{
			reg_a = IndirectX(val1);
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0xB1:
		{
			reg_a = IndirectY(val1, true);
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	}

	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(reg_a & (0x1 << 7));

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpLDX() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;
	switch(opcode_current)
	{
	case 0xA2:
		{
			reg_x = val1;
			cycle_local += 2;
			reg_pc += 2;
		}
		break;
	case 0xA6:
		{
			reg_x = ZeroPage(val1);
			cycle_local += 3;
			reg_pc += 2;
		}
		break;
	case 0xB6:
		{
			reg_x = ZeroPageY(val1);
			cycle_local += 4;
			reg_pc += 2;
		}
		break;
	case 0xAE:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			reg_x = Absolute(val1, val2);
			cycle_local += 4;
			reg_pc += 3;
		}
		break;
	case 0xBE:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			reg_x = AbsoluteY(val1, val2, true);
			cycle_local += 4;
			reg_pc += 3;
		}
		break;
	}

	reg_p.SetZeroFlag(reg_x == 0);
	reg_p.SetNegativeFlag(reg_x & (0x1 << 7));

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpLDY() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;

	switch(opcode_current)
	{
	case 0xA0:
		{
			reg_y = val1;
			reg_pc += 2;
			cycle_local += 2;
		}
		break;
	case 0xA4:
		{
			reg_y = ZeroPage(val1);
			reg_pc += 2;
			cycle_local += 3;
		}
		break;
	case 0xB4:
		{
			reg_y = ZeroPageX(val1);
			reg_pc += 2;
			cycle_local += 4;
		}
		break;
	case 0xAC:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			reg_y = Absolute(val1, val2);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0xBC:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			reg_y = AbsoluteX(val1, val2, true);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	}

	reg_p.SetZeroFlag(reg_y == 0);
	reg_p.SetNegativeFlag(reg_y & (0x1 << 7));
//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpLSR() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;
	uint8 tv;

	switch(opcode_current)
	{
	case 0x4A: //accumulator addressing
		{
			reg_p.SetCarryFlag(reg_a & 0x1);
			reg_a >>= 1;
			reg_p.SetZeroFlag(reg_a == 0);
			reg_p.SetNegativeFlag(reg_a & (0x1 << 7));

			reg_pc += 1;
			cycle_local += 2;
		}
		break;
	case 0x46:
		{
			tv = ZeroPage(val1);
			reg_p.SetCarryFlag(tv & 0x1);
			tv >>= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(tv & (0x1 << 7));

			//write back to memory
			ZeroPageWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	case 0x56:
		{
			tv = ZeroPageX(val1);
			reg_p.SetCarryFlag(tv & 0x1);
			tv >>= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(tv & (0x1 << 7));

			//write back to memory
			ZeroPageXWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0x4E:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);

			tv = Absolute(val1, val2);
			reg_p.SetCarryFlag(tv & 0x1);
			tv >>= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(tv & (0x1 << 7));

			//write back to memory
			AbsoluteWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 6;
		}
		break;
	case 0x5E:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);

			tv = AbsoluteX(val1, val2, false);
			reg_p.SetCarryFlag(tv & 0x1);
			tv >>= 1;
			reg_p.SetZeroFlag(tv == 0);
			reg_p.SetNegativeFlag(tv & (0x1 << 7));

			//write back to memory
			AbsoluteXWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 7;
		}
		break;
	}


	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpNOP() 
{
	//EA

	switch(opcode_current)
	{
	case 0xEA:
	case 0x1A:
	case 0x3A:
		{
			reg_pc+=1;
			cycle_local +=2;
		}
		break;
	case 0x80:
	case 0x82:
		{
			reg_pc+=1;
			cycle_local +=2;
		}
		break;
	}
	



	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpORA() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;
	uint8 tv;

	switch(opcode_current)
	{
	case 0x09:
		{
			tv = val1;
			reg_pc += 2;
			cycle_local += 2;
		}
		break;
	case 0x05:
		{
			tv = ZeroPage(val1);
			reg_pc += 2;
			cycle_local += 3;
		}
		break;
	case 0x15:
		{
			tv = ZeroPageX(val1);
			reg_pc += 2;
			cycle_local += 4;
		}
		break;
	case 0x0D:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			tv = Absolute(val1, val2);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0x1D:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			tv = AbsoluteX(val1, val2, true);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0x19:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			tv = AbsoluteY(val1, val2, true);
			reg_pc += 3;
			cycle_local += 4;
		}
		break;
	case 0x01:
		{
			tv = IndirectX(val1);
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0x11:
		{
			tv = IndirectY(val1, true);
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	}

	reg_a = reg_a | tv;

	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(reg_a & (0x1 << 7));

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpPHA() 
{
	PushStack8(reg_a);
	reg_pc += 1;
	cycle_local +=3;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpPHP() 
{
	reg_p.SetUnusedFlag(true);
	PushStack8(reg_p.ToByte() | 0x30);
	reg_pc +=1;
	cycle_local +=3;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpPLA() 
{
	reg_a = PopStack8();
	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_a));
	reg_pc += 1;
	cycle_local += 4;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpPLP() 
{
	uint8 val = PopStack8();
	reg_p.FromByte(val);
	reg_p.SetUnusedFlag(true);
	reg_pc += 1;
	cycle_local += 4;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpROL() 
{
	//printf("ROL\n");
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	uint8 tv;

	switch(opcode_current)
	{
	case 0x2A:
		{
			//read
			tv = reg_a;

			if(reg_p.GetCarryFlag())
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = (tv << 1) | 1;
			}
			else
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = tv << 1;
			}
			//write
			reg_a = tv;

			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);

			reg_pc += 1;
			cycle_local += 2;
		}
		break;
	case 0x26:
		{
			//read
			tv = ZeroPage(val1);

			if(reg_p.GetCarryFlag())
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = (tv << 1) | 1;
			}
			else
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = tv << 1;
			}
			//write
			ZeroPageWrite(val1, tv);

			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);

			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	case 0x36:
		{
			//read
			tv = ZeroPageX(val1);

			if(reg_p.GetCarryFlag())
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = (tv << 1) | 1;
			}
			else
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = tv << 1;
			}
			//write
			ZeroPageXWrite(val1, tv);

			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);

			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0x2E:
		{
			//read
			tv = Absolute(val1, val2);

			if(reg_p.GetCarryFlag())
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = (tv << 1) | 1;
			}
			else
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = tv << 1;
			}
			//write
			AbsoluteWrite(val1, val2, tv);

			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);



			reg_pc += 3;
			cycle_local += 6;
		}
		break;
	case 0x3E:
		{
			//read
			tv = AbsoluteX(val1, val2, false);

			if(reg_p.GetCarryFlag())
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = (tv << 1) | 1;
			}
			else
			{
				reg_p.SetCarryFlag((tv & 0x80) == 0x80);
				tv = tv << 1;
			}
			//write
			AbsoluteXWrite(val1, val2, tv);

			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);


			reg_pc += 3;
			cycle_local += 7;
		}
		break;
	}


	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpROR() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);

	switch(opcode_current)
	{
	case 0x6A:
		{
			bool oldCarry = reg_p.GetCarryFlag();
			reg_p.SetCarryFlag(reg_a & 0x1);
			reg_a >>= 1;
			reg_a |= ((uint8)oldCarry & 0x1) << 7;
			reg_p.SetNegativeFlag(IS_NEG(reg_a));
			reg_p.SetZeroFlag(reg_a == 0);
			reg_pc += 1;
			cycle_local += 2;
		}
		break;
	case 0x66:
		{
			uint8 tv = ZeroPage(val1);
			bool oldCarry = reg_p.GetCarryFlag();
			reg_p.SetCarryFlag((tv & 0x1));
			tv >>= 1;
			tv |= ((uint8)oldCarry & 0x1) << 7;
			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);
			ZeroPageWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 5;
		}
		break;
	case 0x76:
		{
			uint8 tv = ZeroPageX(val1);
			bool oldCarry = reg_p.GetCarryFlag();
			reg_p.SetCarryFlag((tv & 0x1));
			tv >>= 1;
			tv |= ((uint8)oldCarry & 0x1) << 7;
			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);
			ZeroPageXWrite(val1, tv);
			reg_pc += 2;
			cycle_local += 6;
		}
		break;
	case 0x6E:
		{
			uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
			uint8 tv = Absolute(val1, val2);
			bool oldCarry = reg_p.GetCarryFlag();
			reg_p.SetCarryFlag((tv & 0x1));
			tv >>= 1;
			tv |= ((uint8)oldCarry & 0x1) << 7;
			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);
			AbsoluteWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 6;
		}
		break;
	case 0x7E:
		{
			uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
			uint8 tv = AbsoluteX(val1, val2, false);
			bool oldCarry = reg_p.GetCarryFlag();
			reg_p.SetCarryFlag((tv & 0x1));
			tv >>= 1;
			tv |= ((uint8)oldCarry & 0x1) << 7;
			reg_p.SetNegativeFlag(IS_NEG(tv));
			reg_p.SetZeroFlag(tv == 0);
			AbsoluteXWrite(val1, val2, tv);
			reg_pc += 3;
			cycle_local += 7;
		}
		break;
	}


	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpRTI() 
{
	uint8 val1 = PopStack8();
	reg_p.FromByte(val1);
	reg_p.SetUnusedFlag(true);
	reg_pc = PopStack16();

	cycle_local += 6;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpRTS() 
{
	reg_pc = PopStack16();
	cycle_local += 6;
	reg_pc += 1;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpSBC() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
	uint8 tv;
	switch(opcode_current)
	{
	case 0xFF: //ISB illegal (AbsX)
		{
			tv = AbsoluteX(val1, val2, true);
			tv++;
			AbsoluteXWrite(val1, val2, tv);
			reg_pc += 3; cycle_local += 7;
		}
		break;
	case 0xE3:
		{
			tv = IndirectX(val1);
			tv++;
			IndirectXWrite(val1, tv);
			reg_pc += 2; cycle_local += 8;
		}
		break;
	case 0xEB: //illegal ++
	case 0xE9:
		tv = val1; reg_pc += 2; cycle_local += 2;
		break;
	case 0xE5:
		tv = ZeroPage(val1); reg_pc += 2; cycle_local += 3;
		break;
	case 0xF5:
		tv = ZeroPageX(val1); reg_pc += 2; cycle_local += 4;
		break;
	case 0xED:
		tv = Absolute(val1, val2); reg_pc += 3; cycle_local += 4;
		break;
	case 0xFD:
		tv = AbsoluteX(val1, val2, true); reg_pc += 3; cycle_local += 4;
		break;
	case 0xF9:
		tv = AbsoluteY(val1, val2, true); reg_pc += 3; cycle_local += 4;
		break;
	case 0xE1:
		tv = IndirectX(val1); reg_pc += 2; cycle_local += 6;
		break;
	case 0xF1:
		tv = IndirectY(val1, true); reg_pc += 2; cycle_local += 5;
		break;
	}

	uint32 tv_big = uint32(reg_a) - uint32(tv);
	if(!reg_p.GetCarryFlag())
		tv_big--;

	reg_p.SetCarryFlag(tv_big < 0x100);
	reg_p.SetOverflowFlag(((reg_a ^ tv) & (reg_a ^ tv_big) & 0x80) == 0x80);


	reg_a = tv_big & 0xff;
	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_a));


	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpSEC() 
{
	reg_p.SetCarryFlag(true);
	reg_pc+=1;
	cycle_local += 2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpSED() 
{
	reg_p.SetDecimalModeFlag(true);
	reg_pc += 1;
	cycle_local+=2;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpSEI() 
{
	reg_p.SetInterruptDisableFlag(true);
	cycle_local +=2;
	reg_pc++;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready++++++++++
void CPU::OpSTA() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;

	switch(opcode_current)
	{
	case 0x85:
		{
			ZeroPageWrite(val1, reg_a);
			cycle_local+=3;
			reg_pc+=2;
		}
		break;
	case 0x95:
		{
			ZeroPageXWrite(val1, reg_a);
			cycle_local+=4;
			reg_pc+=2;
		}
		break;
	case 0x8D:
		{
			val2 =  pNes->ReadMemory8(reg_pc + 2);
			AbsoluteWrite(val1, val2, reg_a);
			cycle_local += 4;
			reg_pc += 3;
		}
		break;
	case 0x9D:
		{
			val2 =  pNes->ReadMemory8(reg_pc + 2);
			AbsoluteXWrite(val1, val2, reg_a);
			cycle_local += 5;
			reg_pc += 3;
		}
		break;
	case 0x99:
		{
			val2 =  pNes->ReadMemory8(reg_pc + 2);
			AbsoluteYWrite(val1, val2, reg_a);
			cycle_local += 5;
			reg_pc += 3;
		}
		break;
	case 0x81:
		{
			IndirectXWrite(val1, reg_a);
			cycle_local+=6;
			reg_pc+=2;
		}
		break;
	case 0x91:
		{
			IndirectYWrite(val1, reg_a);
			cycle_local+=6;
			reg_pc+=2;
		}
		break;
	}
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpSTX() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	switch(opcode_current)
	{
	case 0x86:
		{
			ZeroPageWrite(val1, reg_x);
			reg_pc += 2;
			cycle_local +=3;
		}
		break;
	case 0x96:
		{
			ZeroPageYWrite(val1, reg_x);
			reg_pc += 2;
			cycle_local +=4;
		}
		break;
	case 0x8E:
		{
			uint8 val2 = pNes->ReadMemory8(reg_pc + 2);
			AbsoluteWrite(val1, val2, reg_x);
			reg_pc += 3;
			cycle_local +=4;
		}
		break;
	}
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpSTY() 
{
	uint8 val1 = pNes->ReadMemory8(reg_pc + 1);
	uint8 val2;

	switch(opcode_current)
	{
	case 0x84:
		{
			ZeroPageWrite(val1, reg_y);
			reg_pc+=2;
			cycle_local+=3;
		}
		break;
	case 0x94:
		{
			ZeroPageXWrite(val1, reg_y);
			reg_pc+=2;
			cycle_local+=4;
		}
		break;
	case 0x8C:
		{
			val2 = pNes->ReadMemory8(reg_pc + 2);
			AbsoluteWrite(val1, val2, reg_y);
			reg_pc+=3;
			cycle_local+=4;
		}
		break;
	}
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpTAX() 
{
	reg_x = reg_a;
	reg_p.SetZeroFlag(reg_x == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_x));
	reg_pc+=1;
	cycle_local +=2;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpTAY() 
{
	reg_y = reg_a;
	reg_p.SetZeroFlag(reg_y == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_y));
	reg_pc+=1;
	cycle_local+=2;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpTSX() 
{
	
	reg_x = reg_sp;
	reg_p.SetZeroFlag(reg_x == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_x));
	reg_pc+=1;
	cycle_local +=2;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready++++++++++
void CPU::OpTXA() 
{
	reg_a = reg_x;
	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_a));
	reg_pc+=1;
	cycle_local +=2;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready+
void CPU::OpTXS() 
{
	
	reg_sp = reg_x;
	cycle_local+=2;
	reg_pc+=1;
	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}
//ready
void CPU::OpTYA() 
{
	reg_a = reg_y;
	reg_p.SetZeroFlag(reg_a == 0);
	reg_p.SetNegativeFlag(IS_NEG(reg_a));
	reg_pc+=1;
	cycle_local +=2;

	//printf("%s [%02x]\tA:%02x X:%02x Y:%02x SP:%02x \n\0", __FUNCTION__, opcode_current, reg_a, reg_x, reg_y, reg_sp);   
}