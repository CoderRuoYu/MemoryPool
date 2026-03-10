#pragma once
#include "Common.hpp"
#include "CentralCache.hpp"

class ThreadCache
{
public:
	void* Allocate(size_t size)
	{
		size_t index = SizeClass::Index(size);
		size_t roundUpSize = SizeClass::RoundUp(size);
		//队列中有数据
		if (!_freeList[index].IsEmpty())
		{
			return _freeList[index].Pop();
		}
		else
		{
			//这个size传递是的是对齐前的
			return FentchFromCentralCache(index, roundUpSize);
		}
	}
	void Deallocate(void* mem, size_t size)
	{
		assert(mem != nullptr);
		size_t index = SizeClass::Index(size);
		_freeList[index].Push(mem);
	}
	void* FentchFromCentralCache(size_t index, size_t size)
	{
		size_t batchNum = std::min(_freeList[index].GetMaxSize(), SizeClass::SizeToBatchNum(size));
		if (_freeList[index].GetMaxSize() == batchNum)
		{
			_freeList->GetMaxSize()++;
		}
		if (!CentralCache::GetCentralCacheInstance().IsEmpty(index))
		{
			//不为空，先找到一个非空span
		}
		else
		{
			
		}



		
	}

private:
	FreeList _freeList[NFreeLists];
};
thread_local ThreadCache* ptrThreadCache = nullptr;
