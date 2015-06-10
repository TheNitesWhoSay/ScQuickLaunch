#include "Config.h"
#include <iostream>

void displayOptionalFlags()
{
	std::cout << "Accepted Program Arguments:" << std::endl
			  << "	-ke killExplorer" << std::endl
			  << "	-de ddemulate (not compatible with windowed or custom start)" << std::endl
			  << "	-fm fastMenus" << std::endl
			  << "	-zt zeroTimer" << std::endl
			  << "	-nl noLat" << std::endl
			  << "	-w  windowed (not compatible with custom start)" << std::endl
			  << "	-cs customStart (place code to start SC in CustomStarter.bat)" << std::endl
			  << "	-pl postLoad (place code to run after SC window's open in PostLoad.bat)" << std::endl
			  << "	-rm runMap (next argument must be the map path)" << std::endl << std::endl;
}

void parseFlags( int argc, const char* argv[], const char* &mapPath,
				 bool &killExplorer, bool &ddEmulate, bool &fastMenus, bool &zeroTimer,
				 bool &noLat, bool &windowed, bool &customStart, bool &postLoad, bool &runMap )
{
	if ( argc > 1 )
		std::cout << "Given Arguments: ";
	else
		std::cout << "No Arguments Given.";

	for ( int i=1; i<argc; i++ )
	{
		bool isAFlag = true;
		if ( strcmp(argv[i], "-ke") == 0 )
			killExplorer = true;
		else if ( strcmp(argv[i], "-de") == 0 )
			ddEmulate = true;
		else if ( strcmp(argv[i], "-fm") == 0 )
			fastMenus = true;
		else if ( strcmp(argv[i], "-zt") == 0 )
			zeroTimer = true;
		else if ( strcmp(argv[i], "-nl") == 0 )
			noLat = true;
		else if ( strcmp(argv[i], "-w") == 0 )
			windowed = true;
		else if ( strcmp(argv[i], "-cs") == 0 )
			customStart = true;
		else if ( strcmp(argv[i], "-pl") == 0 )
			postLoad = true;
		else if ( strcmp(argv[i], "-rm")  == 0 && i+1 < argc )
		{
			runMap = true;
			mapPath = argv[i+1];
		}
		else
			isAFlag = false;

		if ( isAFlag )
			std::cout << argv[i] << ' ';
	}
	std::cout << std::endl << std::endl;
}

DWORD GetSubKeyString(HKEY hParentKey, const char* subKey, const char* valueName, char* data, DWORD* dataSize)
{
	HKEY hKey = NULL;

	DWORD errorCode = RegOpenKeyEx(hParentKey, subKey, 0, KEY_QUERY_VALUE, &hKey); // Open key
	if ( errorCode == ERROR_SUCCESS ) // Open key success
	{
		errorCode = RegQueryValueEx(hKey, valueName, NULL, NULL, (LPBYTE)data, dataSize); // Read Key
		RegCloseKey(hKey); // Close key
	}
	return errorCode; // Return success/error message
}

bool GetRegString(char* dest, unsigned int destSize, const char* subKey, const char* valueName)
{
	DWORD lpcbData = (DWORD)destSize;

	DWORD errorCode = GetSubKeyString(HKEY_CURRENT_USER, subKey, valueName, dest, &lpcbData); // Try HKCU
	if ( errorCode != ERROR_SUCCESS ) // Not found in HKCU
		errorCode = GetSubKeyString(HKEY_LOCAL_MACHINE, subKey, valueName, dest, &lpcbData); // Try HKLM

	return errorCode == ERROR_SUCCESS;
}

bool GetRegScPath(char* dest, unsigned int destSize)
{
	return GetRegString(dest, destSize, "SOFTWARE\\Blizzard Entertainment\\Starcraft", "InstallPath");
}

bool GetScPath(char* dest, unsigned int destSize)
{
	return GetRegScPath(dest, destSize);
}

bool GetScExePath(char* dest, unsigned int destSize)
{
	if ( GetScPath(dest, destSize) )
	{
		strcat_s(dest, destSize, "\\StarCraft.exe");
		return true;
	}
	return false;
}
