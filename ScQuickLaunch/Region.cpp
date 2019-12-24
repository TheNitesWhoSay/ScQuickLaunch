#include "Region.h"

Region::Region() : base(NULL), size(0), data(NULL)
{

}

Region::~Region()
{
	if ( data != NULL )
		free(data);
}

bool Region::init(LPCVOID baseAddress, SIZE_T regionSize)
{
	if ( regionSize > 0 )
	{
		base = baseAddress;
		size = regionSize;
		data = malloc(size);

		if ( data != NULL )
			return true;
	}
	return false;
}

bool Region::readIn(ProcessModder* process)
{
	return process->readMem(base, size, data);
}