#ifndef MEMSEARCH_H
#define MEMSEARCH_H
#include <Windows.h>
#include "ProcessModder.h"
#include "Region.h"

class MemSearch
{
	public:

		MemSearch();

		~MemSearch();

		bool NewScan(ProcessModder* newProcess, bool scanIncludeReadOnly); // Loads up memory regions
			// Pre: hSearchProcess has been hooked by this program

		void EndScan();

		void FilterExactly(int value);

		void PrintRegions();

		bool canRead(MEMORY_BASIC_INFORMATION &region);

		bool canWrite(MEMORY_BASIC_INFORMATION &region);

		void printAll(int value);


	protected:

		void FlushRegions();


	private:
		bool includeReadOnly;
		ProcessModder* process; // The process currently being searched
		Region* firstRegion; // Forward list of regions remaining in the search
};

#endif