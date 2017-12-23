#ifndef IMapper_h__
#define IMapper_h__
#include "Globals.h"

class Cartridge;
class IMapper
{
public:
	IMapper(Cartridge* parent){pCart = parent;}
	virtual ~IMapper(){}
	virtual void Init() = 0;
	virtual void Write(uint16 address, uint8 val) = 0;
	virtual void Tick() = 0;
protected:
	Cartridge* pCart;
};
#endif // IMapper_h__
