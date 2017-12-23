#ifndef Mapper0_h__
#define Mapper0_h__
#include "IMapper.h"


class Mapper0 : public IMapper
{
public:
	Mapper0(Cartridge* parent);
	~Mapper0();
	void Init();
	void Write(uint16 address, uint8 val);
	void Tick();
};
#endif // Mapper0_h__
