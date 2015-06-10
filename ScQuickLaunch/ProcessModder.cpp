#include "ProcessModder.h"
#include <AclAPI.h> // For modifying process security settings
#include <TlHelp32.h> // For examining currently open processes
#include <Psapi.h> // For finding process base address
#include <iostream>
using namespace std;

bool ProcessModder::openWithProcessID(DWORD processID)
{
	if ( isOpen() )
		close();

	return Hook(processID);
}

bool ProcessModder::openWithProcessName(const char* szProcessName)
{
	if ( isOpen() )
		close();

	DWORD processID;

	return	  FindProcess(szProcessName, processID)
		   && Hook(processID);
}

bool ProcessModder::openWithWindowName(const char* szWindowName)
{
	if ( isOpen() )
		close();

	DWORD processID;

	HWND hWindow = FindWindow(NULL, szWindowName);
	
	return	  GetWindowThreadProcessId(hWindow, &processID)
		   && Hook(processID);
}

bool ProcessModder::openWithProcessHandle(HANDLE hProcess)
{
	if ( isOpen() )
		close();

	DWORD processID = GetProcessId(hProcess);

	return	  processID != 0
		   && Hook(processID);
}

bool ProcessModder::isOpen()
{
	DWORD lpExitCode;

	return	  hookedProcess != NULL
		   && GetExitCodeProcess(hookedProcess, &lpExitCode)
		   && lpExitCode == STILL_ACTIVE;
}

void ProcessModder::close()
{
	if ( isOpen() )
	{
		CloseHandle(hookedProcess);
		hookedProcess = NULL;
	}
}

template <typename valueType> // Allow writing to different data types
bool ProcessModder::writeMem(UINT address, valueType value)
{
	return WriteProcessMemory(hookedProcess, (LPVOID)address, &value, sizeof(valueType), NULL) == TRUE;
}
template bool ProcessModder::writeMem<UINT>(UINT address, UINT value); // Explicit instantiations
template bool ProcessModder::writeMem<USHORT>(UINT address, USHORT value);
template bool ProcessModder::writeMem<BYTE>(UINT address, BYTE value);

template <typename valueType> // Allow reading from different data types
bool ProcessModder::readMem(UINT address, valueType &value)
{
	if ( ReadProcessMemory(hookedProcess, (LPCVOID)address, &value, sizeof(valueType), NULL) == TRUE )
		return true;
	else
	{
		value = 0; // Ensure value is initialized for safety
		return false;
	}
}
template bool ProcessModder::readMem<int>(UINT address, int &value);
template bool ProcessModder::readMem<UINT>(UINT address, UINT &value); // Explicit instantiations
template bool ProcessModder::readMem<USHORT>(UINT address, USHORT &value);
template bool ProcessModder::readMem<BYTE>(UINT address, BYTE &value);

bool ProcessModder::readMem(LPCVOID baseAddress, UINT numBytes, void* dest)
{
	return ReadProcessMemory(hookedProcess, baseAddress, dest, numBytes, NULL) == TRUE;
}

bool ProcessModder::GetArrayAddress(LPVOID &address, UINT* arrayItems, UINT arrayLength)
{
	return false;
}

bool ProcessModder::Hook(DWORD processID)
	// Attempt to open file with r/w access
	// If that fails, try to replace DAC information
	// Retry opening process with r/w access
{
	hookedProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID); // Open process with full access
	if ( hookedProcess == NULL )
	{
		hookedProcess = OpenProcess(PROCESS_ALL_LEGACY_ACCESS, false, processID); // Retry with access compatible with legacy operating systems

		if ( hookedProcess == NULL ) // Some security is probably getting in the way, destroy it
		{
			HANDLE pHandle = OpenProcess(WRITE_DAC, false, processID); // Open process with DACL access
			if ( pHandle != NULL ) // Successfully gained DACL access
			{
				PSECURITY_DESCRIPTOR secdesc; // Placeholder (required)
				PACL dacl = NULL; // Pointer to access information (ie: whether you can read the process's memory)

				HANDLE pCurr = GetCurrentProcess();
				if (	( GetSecurityInfo(pCurr, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,	// Get the security information for this process
										  NULL, NULL, &dacl, NULL, &secdesc) == ERROR_SUCCESS )
					 && ( SetSecurityInfo(pHandle, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION,
										  NULL, NULL, dacl, NULL) == ERROR_SUCCESS ) ) // Replace the target process's security information with this process's security information
				{
					hookedProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID); // Open process with full access
					if ( hookedProcess == NULL )
						hookedProcess = OpenProcess(PROCESS_ALL_LEGACY_ACCESS, false, processID); // Retry with access compatible with legacy operating systems
				}

				CloseHandle(pHandle); // Close the process handle with DACL access
				LocalFree(secdesc); // Free the placeholder
			}
		}
	}
	return hookedProcess != NULL; // Return true if successfully opened
}

bool FindProcess(const char* szProcessName, DWORD &processID)
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = { }; // Holds information about a process
	pe32.dwSize = sizeof(PROCESSENTRY32);

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Get list of running processes
	if ( hProcessSnap != INVALID_HANDLE_VALUE ) // Got list successfully
	{
		if ( Process32First(hProcessSnap, &pe32) ) // If you can get information about the first process
		{
			do // Step through processes
			{
				if ( strcmp(szProcessName, pe32.szExeFile) == 0 ) // If process name is the name you're searching for
				{
					processID = pe32.th32ProcessID; // Set returned process ID value
					CloseHandle(hProcessSnap); // Cleanup process list
					return true;
				}
			} while ( Process32Next(hProcessSnap, &pe32) ); // While there's more processes
		}

		CloseHandle(hProcessSnap); // Cleanup process list
	}

	return false;
}

bool ProcessModder::GetBaseAddress(LPVOID &address)
{
	if ( isOpen() )
	{
		char hookedProcessName[MAX_PATH];
		DWORD hookedProcessNameSize = MAX_PATH;
		if ( QueryFullProcessImageName(hookedProcess, 0, hookedProcessName, &hookedProcessNameSize) ) // Get hooked process name
		{
			HMODULE modules[MAX_MODULES]; // Array to store open modules
			DWORD moduleBytes; // Number of bytes used in module array
			if ( EnumProcessModules(hookedProcess, modules, MAX_MODULES*sizeof(HMODULE), &moduleBytes) ) // Fill module array
			{
				HMODULE moduleHandle = NULL; // Handle to module being examined
				for ( int i=0; i<(moduleBytes/sizeof(HMODULE)); i++ ) // Cycle through all modules
				{
					char examinedName[MAX_PATH]; // Name of module being examined
					if ( GetModuleFileNameEx(hookedProcess, modules[i], examinedName, MAX_PATH) ) // Get name of module being examined
					{
						if ( strcmp(examinedName, hookedProcessName) == 0 ) // Name is identical to name of hooked module
						{
							MODULEINFO moduleInfo; // Information about hooked module
							if ( GetModuleInformation(hookedProcess, moduleHandle, &moduleInfo, sizeof(MODULEINFO)) ) // Get info about hooked module
							{
								address = moduleInfo.EntryPoint; // Get base address
								return true;
							}
						}
					}
				}
			}
		}
	}
	address = 0; // Ensure value is initialized to be... slightly safer
	return false;
}

bool ProcessModder::GetMemRegionSize(UINT startAddress, UINT &size)
{
	if ( isOpen() )
	{
		MEMORY_BASIC_INFORMATION memInfo;
		SIZE_T memInfoStructSize = sizeof(memInfo);
		if ( VirtualQueryEx(hookedProcess, (LPCVOID)startAddress, &memInfo, memInfoStructSize) )
		{
			size = memInfo.RegionSize;
			return true;
		}
	}

	size = 0;
	return false;
}

void ProcessModder::printThreads()
{
	if ( isOpen() )
	{
		DWORD processID = GetProcessId(hookedProcess);
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, processID);

		if ( snapshot != INVALID_HANDLE_VALUE )
		{
			THREADENTRY32 thread = { };
			thread.dwSize = sizeof(THREADENTRY32);

			if ( Thread32First(snapshot, &thread) == TRUE )
			{
				do
				{
					if ( thread.th32OwnerProcessID == processID )
					{
						cout << "Thread ID: " << uppercase << hex << thread.th32ThreadID << endl;
					}
				} while ( Thread32Next(snapshot, &thread) );
			}
			CloseHandle(snapshot);
		}
	}
}

void ProcessModder::printAccessibleMemRegions(bool includeReadOnly)
{
	if ( isOpen() )
	{
		UINT position = 0;
		MEMORY_BASIC_INFORMATION regionInfo = { };

		while ( VirtualQueryEx(hookedProcess, (LPCVOID)position, &regionInfo, sizeof(regionInfo)) != 0 )
		{
			PrintMemRegionInfo(regionInfo);
			cout << endl << endl;

			//cin.get();

			position += regionInfo.RegionSize;
		}
	}
}

void ProcessModder::PrintProtectInfo(DWORD protectionFlags)
{
	if ( protectionFlags == 0 )
		cout << "No Access";
	else if ( protectionFlags & PAGE_EXECUTE		   ) cout << "PAGE_EXECUTE";
	else if ( protectionFlags & PAGE_EXECUTE_READ	   ) cout << "PAGE_EXECUTE_READ";
	else if ( protectionFlags & PAGE_EXECUTE_READWRITE ) cout << "PAGE_EXECUTE_READWRITE";
	else if ( protectionFlags & PAGE_EXECUTE_WRITECOPY ) cout << "PAGE_EXECUTE_WRITECOPY";
	else if ( protectionFlags & PAGE_NOACCESS		   ) cout << "PAGE_NOACCESS";
	else if ( protectionFlags & PAGE_READONLY		   ) cout << "PAGE_READONLY";
	else if ( protectionFlags & PAGE_READWRITE		   ) cout << "PAGE_READWRITE";
	else if ( protectionFlags & PAGE_WRITECOPY		   ) cout << "PAGE_WRITECOPY";
	else
		cout << "Undefined";

	if ( protectionFlags & PAGE_GUARD		 ) cout << "|PAGE_GUARD";
	if ( protectionFlags & PAGE_NOCACHE		 ) cout << "|PAGE_NOCACHE";
	if ( protectionFlags & PAGE_WRITECOMBINE ) cout << "|PAGE_WRITECOMBINE";
}

void ProcessModder::PrintMemRegionInfo(MEMORY_BASIC_INFORMATION &region)
{
	cout << "Base	 : " << uppercase << hex << region.BaseAddress << endl
		 << "aBase	 : " << uppercase << hex << region.AllocationBase << endl
		 << "aProtect : ";

	PrintProtectInfo(region.AllocationProtect);

	cout << endl
		 << "Size	 : " << uppercase << hex << region.RegionSize << endl
		 << "State	 : ";

	if		( region.State & MEM_COMMIT	 ) cout << "MEM_COMMIT";
	else if ( region.State & MEM_FREE	 ) cout << "MEM_FREE";
	else if ( region.State & MEM_RESERVE ) cout << "MEM_RESERVE";
	else
		cout << "Undefined";

	cout << endl
		 << "Protect	 : ";

	PrintProtectInfo(region.Protect);

	cout << endl
		 << "Type	 : ";

	if		( region.Type & MEM_IMAGE	) cout << "MEM_IMAGE";
	else if ( region.Type & MEM_MAPPED	) cout << "MEM_MAPPED";
	else if ( region.Type & MEM_PRIVATE ) cout << "MEM_PRIVATE";
	else
		cout << "Undefined";
}