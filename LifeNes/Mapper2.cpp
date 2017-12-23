#include "Mapper2.h"
#include "Cartridge.h"

Mapper2::Mapper2(Cartridge* parent)
	:IMapper(parent)
{

}
Mapper2::~Mapper2(){

}
void Mapper2::Init()
{
	pCart->Mapper_SwitchPrg16((pCart->prg_page_count - 1) * 4, 1);
	pCart->Mapper_SwitchChr8(0);
}
void Mapper2::Write(uint16 address, uint8 val)
{
	if(address >= 0x8000 && address <= 0xffff)
	{
		pCart->Mapper_SwitchPrg16(val * 4, 0);
	}
}
void Mapper2::Tick()
{

}