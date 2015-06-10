#ifndef CONFIG_H
#define CONFIG_H
#include <Windows.h>

void displayOptionalFlags();

void parseFlags( int argc, const char* argv[], const char* &mapPath,
				 bool &killExplorer, bool &ddEmulate, bool &fastMenus, bool &zeroTimer,
				 bool &noLat, bool &windowed, bool &customStart, bool &postLoad, bool &runMap );

DWORD GetSubKeyString(HKEY hParentKey, const char* subKey, const char* valueName, char* data, DWORD* dataSize);

bool GetRegString(char* dest, unsigned int destSize, const char* subKey, const char* valueName);

bool GetRegScPath(char* dest, unsigned int destSize);

bool GetScPath(char* dest, unsigned int destSize);

bool GetScExePath(char* dest, unsigned int destSize);

#endif