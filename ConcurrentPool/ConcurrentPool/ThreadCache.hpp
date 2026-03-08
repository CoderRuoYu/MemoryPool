#pragma once
#include "Common.hpp"

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
			//这个size传递是对齐后的还是对齐前的 还要思考！！！！！！！
			return FentchFromCentralCache(index, size);
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
		//等待填充
		return nullptr;
	}

private:
	FreeList _freeList[NFreeLists];
};
thread_local ThreadCache* ptrThreadCache = nullptr;
