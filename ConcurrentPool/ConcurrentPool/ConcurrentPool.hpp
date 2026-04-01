#pragma once
#include "Common.hpp"
#include "ThreadCache.hpp"
void* AllocateMemory(size_t size)
{
	if (ptrThreadCache == nullptr)
	{
		ptrThreadCache = new ThreadCache;
	}
	return ptrThreadCache->Allocate(size);
}
void DeallocateMemory(void* mem, size_t size)
{
	assert(ptrThreadCache != nullptr);

	ptrThreadCache->Deallocate(mem, size);
}
