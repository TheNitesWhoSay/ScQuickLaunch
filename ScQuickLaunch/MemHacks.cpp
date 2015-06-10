#include "MemHacks.h"
#include <iostream>

void PatchMenus(ProcessModder &process)
{
	for ( int i = 0; i < 43; ++i )
		process.writeMem<USHORT>(0x005129EC+i*4+2, 4);

	for ( int i = 0; i < 5; ++i )
	{
		process.writeMem<USHORT>(0x0051A9F0+i*4+2, 4);
		process.writeMem<USHORT>(0x0051A490+i*4+2, 4);
	}

	process.writeMem<USHORT>(0x00512B10+2, 4);
	process.writeMem<USHORT>(0x00512B10+6, 4);
	process.writeMem<USHORT>(0x0051A844+2, 4);
}

void ZeroScTimer(ProcessModder &process)
{
	/*UINT countdown = 0;
	if ( process.readMem<UINT>(0x0066FBFC, countdown) )
	{
		if ( countdown > 6 ) // Check if start game countdown timer > 6
		{
			if ( process.writeMem<UINT>(0x0066FBFC, 6) ) // Set it to 6 if it's above that
				std::cout << "Timer lowered." << std::endl;
			else
				std::cout << "Timer could not be lowered." << std::endl;
		}
	}*/

	/**
			eax = timerVal
			eax --
			timerVal = eax
			if ( timer == 8 )
			{
				eax = 0x6D5A30
				push eax onto stack
				call 0x0044FD30
				{

				}
				eax = 0x0066FBFC
				(byte)0x0066FBFA = 6
			}
			if ( If timer == 4 )
			{
				ecx = 0x00596888
				if ( ecx != 0 )
				{
					(byte)0x0066FBFA = 7
					call 0x00471270
					eax = 0x0066FBFC
				}
				else
					(byte)0x0066FBFA = 8

				if ( eax == 0 )
					return;
				else
					(byte)0x0066FBFA = 9
			}
	*/
}

void NoLat(ProcessModder &process)
{
	int latency = 0;
	if ( process.readMem<int>(0x006556E4, latency) && latency != -1 ) // Check if latency != -1
	{
		if ( process.writeMem<UINT>(0x006556E4, -1) ) // Set it to 1 if it's not that
			std::cout << "Lat zero'd successfully." << std::endl;
		else
			std::cout << "Lat could not be zero'd." << std::endl;
	}
}

void RunMap(ProcessModder &process, const char* mapPath)
{
	std::cout << "Run map not yet supported, given map: " << mapPath << std::endl;
}
