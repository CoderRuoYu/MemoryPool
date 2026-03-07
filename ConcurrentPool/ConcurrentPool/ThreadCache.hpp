#pragma once
#include "Common.hpp"

class ThreadCache
{
private:
	void* Allocate(size_t size)
	{
		
	}
	void Deallocate()
	{

	}
	void* FentchFromCentralCache(size_t index, size_t size)
	{

	}

public:
	FreeList _freeList[NFreeLists];
};

