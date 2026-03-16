#pragma once
#include "Common.hpp"
#include "CentralCache.hpp"

class ThreadCache
{
public:
	void* Allocate(size_t size)
	{
		size_t index = SizeClass::Index(size);
		//队列中有数据
		if (!_freeList[index].IsEmpty())
		{
			return _freeList[index].Pop();
		}
		else
		{
			return FentchFromCentralCache(index, SizeClass::RoundUp(size));
		}
	}
	void Deallocate(void* mem, size_t size)
	{
		assert(mem != nullptr);
		size_t index = SizeClass::Index(size);
		_freeList[index].Push(mem);
	}
	//返回一个对象，剩下的挂在对应的index链表上
	void* FentchFromCentralCache(size_t index, size_t size)
	{
		size_t batchNum = min(_freeList[index].GetMaxSize(), SizeClass::SizeToBatchNum(size));
		if (_freeList[index].GetMaxSize() == batchNum)
		{
			_freeList->GetMaxSize()++;
		}

		void* begin = nullptr;
		void* end = nullptr;
		size_t actualNum = CentralCache::GetCentralCacheInstance().FetchRangeObj(begin, end, batchNum, size);
		assert(actualNum > 0);
		if (actualNum == 1)
		{
			return begin;
		}
		else
		{
			void* res = begin;
			begin = NextObj(begin);
			_freeList[index].PushRangeObj(begin, end);
			return res;
		}
	}

private:
	FreeList _freeList[NFreeLists];
};
thread_local ThreadCache* ptrThreadCache = nullptr;
