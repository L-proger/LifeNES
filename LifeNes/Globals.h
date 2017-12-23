#ifndef Globals_h__
#define Globals_h__

#define IS_NEG(val) \
	((## val ## & (0x1 << 7)) != 0)

enum REGION
{
	NTSC = 0,
	PAL = 1
};

enum MIRRORING_MODE
{
	MIRROR_HORIZONTAL = 0,
	MIRROR_VERTICAL = 1,
	MIRROR_FOURSCREEN = 2
};

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef char sint8;
typedef short sint16;
typedef int sint32;

static uint16 MakeAddress16(uint8 b1, uint8 b2)
{
	uint16 a = (uint16(b2) << 8) | b1;
	return a;
}

/*sint8 noise(uint8 xor_tap)
{
	static int sreg = 0x4000;
	int bit0, tap, bit14;

	bit0 = sreg & 1;
	tap = (sreg & xor_tap) ? 1 : 0;
	bit14 = (bit0 ^ tap);
	sreg >>= 1;
	sreg |= (bit14 << 14);
	return (bit0 ^ 1);
}*/



#endif // Globals_h__
