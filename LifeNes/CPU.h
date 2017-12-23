#ifndef CPU_h__
#define CPU_h__

#include "Globals.h"
#include  <stdio.h>

enum INTERRUPT_FLAG
{
	NMI = 1,
	IRQ = 2,
	BRK = 3
};

struct StateRegister
{
public:
	StateRegister()
	{
		c = z = i = d = b = v = n = unused = false;
	}
	void SetCarryFlag(bool condition)
	{
		c = condition;
	}
	bool GetCarryFlag()
	{
		return c;
	}
	void SetZeroFlag(bool condition)
	{
		z = condition;
	}
	bool GetZeroFlag()
	{
		return z;
	}
	void SetInterruptDisableFlag(bool condition)
	{
		i = condition;
	}
	bool GetInterruptDisableFlag()
	{
		return i;
	}
	void SetDecimalModeFlag(bool condition)
	{
		d = condition;
	}
	bool GetDecimalModeFlag()
	{
		return d;
	}
	void SetBreakCommandFlag(bool condition)
	{
		b = condition;
	}
	void SetUnusedFlag(bool val)
	{
		unused = val;
	}
	bool GetBreakCommandFlag()
	{
		return b;
	}
	void SetOverflowFlag(bool condition)
	{
		v = condition;
	}
	bool GetOverflowFlag()
	{
		return v;
	}
	void SetNegativeFlag(bool condition)
	{
		n = condition;
	}
	bool GetNegativeFlag()
	{
		return n;
	}
	void FromByte(uint8 flags)
	{
		c = ((flags & (0x1 << 0)) > 0);
		z = ((flags & (0x1 << 1)) > 0);
		i = ((flags & (0x1 << 2)) > 0);
		d = ((flags & (0x1 << 3)) > 0);
		b = ((flags & (0x1 << 4)) > 0);
		unused = ((flags & (0x1 << 5)) > 0);
		v = ((flags & (0x1 << 6)) > 0);
		n = ((flags & (0x1 << 7)) > 0);
	}
	uint8 ToByte()
	{
		uint8 ret = 
			(((uint8)c) << 0) |
			(((uint8)z) << 1) |
			(((uint8)i) << 2) |
			(((uint8)d) << 3) |
			(((uint8)b) << 4) |
			(((uint8)unused) << 5) |
			(((uint8)v) << 6) |
			(((uint8)n) << 7);

		return ret;
	}
private:
	bool c; //0
	bool z; //1
	bool i; //2
	bool d; //3
	bool b; //4
	bool unused; //5
	bool v; //6
	bool n; //7
};
class NES;
class CPU
{
public:
	CPU(NES* parent);
	~CPU();
	uint8 pendingIRQ;
	void PushStack8(uint8 val);
	uint8 PopStack8();
	void PushStack16(uint16 val);
	uint16 PopStack16();
	void Update();
	int Execute();
	int DoCycles(float count);

	uint16 reg_pc;
	uint8 reg_x;
	uint8 reg_y;
	uint8 reg_a;
	StateRegister reg_p;
	uint8 reg_sp;
	int cycle_total;
	int cycle_local;
	uint32 GetCycles()
	{
		return cycle_total;
	}
private:
	
	uint8 opcode_current;
	uint8 opcode_previous;
	NES* pNes;
	typedef void (CPU::*reg_ptr)();
	reg_ptr opcodes[0xff];

	//memory read
	uint8 ZeroPage(uint16 addr);
	uint8 ZeroPageX(uint16 addr);
	uint8 ZeroPageY(uint16 addr);
	uint8 Absolute(uint8 b1, uint8 b2);
	uint8 AbsoluteX(uint8 b1, uint8 b2, bool checkBoundary);
	uint8 AbsoluteY(uint8 b1, uint8 b2, bool checkBoundary);
	uint8 IndirectX(uint8 b1);
	uint8 IndirectY(uint8 b1, bool checkBoundary);

	//memory write
	void ZeroPageWrite(uint16 addr, uint8 data);
	void ZeroPageXWrite(uint16 addr, uint8 data);
	void ZeroPageYWrite(uint16 addr, uint8 data);
	void AbsoluteWrite(uint8 b1, uint8 b2, uint8 data);
	void AbsoluteXWrite(uint8 b1, uint8 b2, uint8 data);
	void AbsoluteYWrite(uint8 b1, uint8 b2, uint8 data);
	void IndirectXWrite(uint8 b1, uint8 data);
	void IndirectYWrite(uint8 b1, uint8 data);

	uint32 internalCycle;

	FILE* pFile;
	char* stringBuffer;

	void OpADC();
	void OpAND();
	void OpASL();
	void OpBCC();
	void OpBCS();
	void OpBEQ();
	void OpBIT();
	void OpBMI();
	void OpBNE();
	void OpBPL();
	void OpBRK();
	void OpBVC();
	void OpBVS();
	void OpCLC();
	void OpCLD();
	void OpCLI();
	void OpCLV();
	void OpCMP();
	void OpCPX();
	void OpCPY();
	void OpDEC();
	void OpDEX();
	void OpDEY();
	void OpEOR();
	void OpINC();
	void OpINX();
	void OpINY();
	void OpISB();
	void OpJMP();
	void OpJSR();
	void OpKIL();
	void OpLAX();
	void OpLDA();
	void OpLDX();
	void OpLDY();
	void OpLSR();
	void OpNOP();
	void OpORA();
	void OpPHA();
	void OpPHP();
	void OpPLA();
	void OpPLP();
	void OpROL();
	void OpROR();
	void OpRTI();
	void OpRTS();
	void OpSBC();
	void OpSEC();
	void OpSED();
	void OpSEI();
	void OpSTA();
	void OpSTX();
	void OpSTY();
	void OpTAX();
	void OpTAY();
	void OpTSX();
	void OpTXA();
	void OpTXS();
	void OpTYA();


};
#endif // CPU_h__
