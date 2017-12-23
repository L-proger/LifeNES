#ifndef Mapper1_h__
#define Mapper1_h__
#include "IMapper.h"
#include "Cartridge.h"

class Mapper1 : public IMapper
{
public:
	Mapper1(Cartridge* parent);
	~Mapper1();
	void Init();
	void Write(uint16 address, uint8 val);
	void Tick();
};
#endif // Mapper1_h__
