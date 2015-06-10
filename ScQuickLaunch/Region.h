#ifndef REGION_H
#define REGION_H
#include <Windows.h>
#include "ProcessModder.h"

#define Address LPCVOID

class Region
{
	public:
		Region(Region* parent);
		~Region();
		bool init(LPCVOID baseAddress, SIZE_T regionSize);
		bool readIn(ProcessModder* process);

		Region* next;
		void* data;

		LPCVOID getBase() { return base; };
		SIZE_T getSize() { return size; };

	private:
		LPCVOID base;
		SIZE_T size;
		
};

#endif