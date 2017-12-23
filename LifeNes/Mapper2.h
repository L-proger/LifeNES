#ifndef Mapper2_h__
#define Mapper2_h__

#include "IMapper.h"
#include "Cartridge.h"

class Mapper2 : public IMapper
{
public:
	Mapper2(Cartridge* parent);
	~Mapper2();
	void Init();
	void Write(uint16 address, uint8 val);
	void Tick();
};
#endif // Mapper2_h__
