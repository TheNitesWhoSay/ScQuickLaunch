#include "MemSearch.h"
#include <new>
#include <iostream>
#include <iomanip> // For stream manipulation
using namespace std;

MemSearch::MemSearch() : process(NULL), includeReadOnly(false)
{

}

MemSearch::~MemSearch()
{
	EndScan();
}

bool MemSearch::NewScan(ProcessModder* newProcess, bool scanIncludeReadOnly)
{
	FlushRegions();
	process = newProcess;
	includeReadOnly = scanIncludeReadOnly;

	if ( process->isOpen() )
	{
		UINT position = 0;
		MEMORY_BASIC_INFORMATION regionInfo = { };
		HANDLE hProcess = process->getHandle();
		std::shared_ptr<Region> curr = nullptr;

		while ( !(VirtualQueryEx(hProcess, (LPCVOID)position, &regionInfo, sizeof(regionInfo)) == 0 && GetLastError() == ERROR_INVALID_PARAMETER) )
		{
			if ( canWrite(regionInfo) || ( includeReadOnly && canRead(regionInfo) ) )
			{
				curr = std::shared_ptr<Region>(new Region());
				if ( !(	   curr->init((LPCVOID)position, regionInfo.RegionSize)
						&& curr->readIn(process)								   ) )
				{
					cout << "Couldn't init or readin" << endl;
				}
				else
				{
                    regions.push_back(curr);
					//cout << uppercase << hex << firstRegion->getBase() << ',' << uppercase << hex << firstRegion->getSize() << endl;
					//cin.get();
				}

				//cout << "Read in area!" << endl;
			}
			//else
				//cout << "Avoided reading area" << endl;
			position += regionInfo.RegionSize;


		}

        return regions.size() > 0;
	}
	return false;
}

void MemSearch::EndScan()
{
	FlushRegions();
}

void MemSearch::PrintRegions()
{
	if ( regions.empty() )
		cout << "No Regions" << endl;

	int totalSize = 0;
	int count = 0;

    for ( auto & region : regions )
    {
		//cout << "Base: 0x" << uppercase << hex << curr->getBase() << " Size: " << uppercase << hex << curr->getSize() << endl;
        int* intData = (int*)(region->getData());
        for ( int i=0; i<region->getSize()/4; i++ )
        {
            if ( intData[i] == 1674 )
            {
				cout << "0x" << setw(8) << setfill('0') << uppercase << hex << UINT(region->getBase())+i*4 << endl;
				count++;
            }
		    totalSize += region->getSize();
        }
    }
	cout << "Total Size: " << dec << totalSize << endl;
	cout << "Count: " << count << endl;
}

bool MemSearch::canRead(MEMORY_BASIC_INFORMATION &region)
{
	return (	 ( region.State & MEM_COMMIT )

			 && (	 ( region.Protect != 0 )
				  || ( region.AllocationProtect != 0 )
				 )

			 && !( region.Protect & PAGE_GUARD )
			 && !( region.AllocationProtect & PAGE_GUARD )

			 && (	 ( region.Protect & PAGE_EXECUTE_READWRITE )
				  || ( region.Protect & PAGE_EXECUTE_WRITECOPY )
				  || ( region.Protect & PAGE_READONLY )
				  || ( region.Protect & PAGE_READWRITE )
				  || ( region.Protect & PAGE_WRITECOPY )
				  || ( region.AllocationProtect & PAGE_EXECUTE_READWRITE )
				  || ( region.AllocationProtect & PAGE_EXECUTE_WRITECOPY )
				  || ( region.AllocationProtect & PAGE_READONLY )
				  || ( region.AllocationProtect & PAGE_READWRITE )
				  || ( region.AllocationProtect & PAGE_WRITECOPY )
				 )
			);
}

bool MemSearch::canWrite(MEMORY_BASIC_INFORMATION &region)
{
	return (	 ( region.State & MEM_COMMIT )

			 && (	 ( region.Protect != 0 )
				  || ( region.AllocationProtect != 0 )
				 )

			 && !( region.Protect & PAGE_GUARD )
			 && !( region.Protect & PAGE_READONLY )

			 && !( region.AllocationProtect & PAGE_GUARD )
			 && !( region.AllocationProtect & PAGE_READONLY )

			 && (	 ( region.Protect & PAGE_READWRITE )
				  || ( region.Protect & PAGE_EXECUTE_READWRITE )
				  || ( region.Protect & PAGE_WRITECOMBINE )
				  || ( region.AllocationProtect & PAGE_READWRITE )
				  || ( region.AllocationProtect & PAGE_EXECUTE_READWRITE )
				  || ( region.AllocationProtect & PAGE_WRITECOMBINE )
				 )
			);
}

void MemSearch::printAll(int value)
{
    for ( auto & region : regions )
    {
		byte* regionDat = (byte*)region->getData();
		// Look through all data in the region
		for ( UINT pos = 0;
			  pos + sizeof(int) < region->getSize();
			  pos ++
			)
		{
			//if ( ((pos + UINT(curr->getBase())) > 0x04010000) && ((pos + UINT(curr->getBase())) < 0x04020000) )
			//{
			if ( (*((int*)&regionDat[pos])) == value )
			{
				cout << "0x" << setw(8) << setfill('0') << uppercase << hex << pos+UINT(region->getBase()) << endl;
			}
			//}
		}
    }
}

void MemSearch::FlushRegions()
{
    regions.clear();
}