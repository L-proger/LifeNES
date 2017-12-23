#include "main.h"

#include <stdio.h>
#include "RenderWindow.h"
#include "NES.h"
#include "Error.h"

#define LOAD_PARAMS

int main(int argc, const char* argv[])
{
	printf("LifeNES emulator by .L\n");

	RenderWindow wnd;

	//256x240 = PAL   256x224 = NTSC
	wnd.SetSize(256*2, 240*2);

	NES* nes = new NES(&wnd);
	nes->SetRegion(NTSC);
	Cartridge* cart = new Cartridge();
	int sc = 0;
#ifdef LOAD_PARAMS
	if(argc > 2)
	{
		printf("Wrong arguments count! Shutting down...\n");
		Sleep(4000);
		exit(0);
	}
	else if(argc < 2)
	{
		printf("Wrong arguments count! ROM Path is unspecified! \nShutting down...\n");
		Sleep(4000);
		exit(0);
	}
	sc = cart->Load(String(argv[1]));
#else
	sc = cart->Load("C:\\NES\\BattleCity.nes");
#endif
	if(sc != SC_OK)
	{
		printf("Invalid ROM file! Shutting down...\n");
		Sleep(3000);
		exit(0);
	}
	//cart->Load("C:\\GALAXIAN.NES");
	
	nes->InsertCartridge(cart);
	nes->Run();

	while(true)
	{
		nes->Update();
		wnd.Update();
	}
	getchar();
}
