#ifndef PROCESSMODDER_H
#define PROCESSMODDER_H
#include <Windows.h>

#define MAX_MODULES 1000

const DWORD PROCESS_ALL_LEGACY_ACCESS = PROCESS_CREATE_PROCESS|PROCESS_CREATE_THREAD|PROCESS_DUP_HANDLE|
										PROCESS_QUERY_INFORMATION|PROCESS_SET_INFORMATION|PROCESS_SET_QUOTA|
										PROCESS_SUSPEND_RESUME|PROCESS_TERMINATE|PROCESS_VM_OPERATION|
										PROCESS_VM_READ|PROCESS_VM_WRITE|SYNCHRONIZE;

/** Finds the process ID that corresponding to the name
	returns true and sets processID if successful */
bool FindProcess(const char* szProcessName, DWORD &processID);

class ProcessModder
{
	public:

		// Constructors / Destructors
		ProcessModder() : hookedProcess(NULL) { } // Default constructor
		~ProcessModder() { close(); } // Destructor

		// Attempts to find and hook the process with the given information
		bool openWithProcessID(DWORD processID);
		bool openWithProcessName(const char* szProcessName);
		bool openWithWindowName(const char* szWindowName);
		bool openWithProcessHandle(HANDLE hProcess);

		// Returns true if a process has been hooked and remains open
		bool isOpen();

		// Cleans up process handles
		void close();

		// Replaces the item at address with 'value'
		template <typename valueType>
			bool writeMem(UINT address, valueType value);

		// Reads the item at address into 'value'
		template <typename valueType>
			bool readMem(UINT address, valueType &value);

		/** Stores all memory from 'startAddress' to 'startAddress'+'numBytes' in dest
			'dest' must be able to hold at least 'numBytes' bytes
			returns true if successful */
		bool readMem(LPCVOID baseAddress, UINT numBytes, void* dest);

		/** Gets the address of an array containing the given items
			returns true if successful */
		bool GetArrayAddress(LPVOID &address, UINT* arrayItems, UINT arrayLength);

		/** Finds the base address of the hooked process
			returns true and sets 'address' if successful */
		bool GetBaseAddress(LPVOID &address);

		/** Finds the amount of contigous memory after startAddress
			returns true and sets size if successful */
		bool GetMemRegionSize(UINT startAddress, UINT &size);

		// Prints threads of the process
		void printThreads();

		// Print all system memory and the access this program has to it
		void printAccessibleMemRegions(bool includeReadOnly);

		// Prints information from the provided memory region structure
		void PrintMemRegionInfo(MEMORY_BASIC_INFORMATION &region);

		HANDLE getHandle() { return hookedProcess; }


	protected:

		// Hooks the process with the given process ID
		bool Hook(DWORD processID);

		
		// Prints details of protection flags from a MEMORY_BASIC_INFORMATION struct
		void PrintProtectInfo(DWORD protectionFlags);


	private:

		HANDLE hookedProcess; // Handle to the currently hooked process
};

#endif