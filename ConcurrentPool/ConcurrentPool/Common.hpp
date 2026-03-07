#pragma once
#include <iostream>
#include <assert.h>
using std::cout;
using std::endl;

static const int NFreeLists = 208;
class FreeList
{
public:
	void* Pop()
	{
		assert(_freeList != nullptr);
		void* cur = _freeList;
		_freeList = *(void**)_freeList;
		return cur;
	}

	void Push(void* cur)
	{
		assert(cur != nullptr);
		*(void**)cur = _freeList;
		_freeList = cur;
	}
private:
	void* _freeList = nullptr;
};
// [1,128] 8byte对⻬ freelist[0,16)
// [128+1,1024] 16byte对⻬ freelist[16,72)
// [1024+1,8*1024] 128byte对⻬ freelist[72,128)
// [8*1024+1,64*1024] 1024byte对⻬ freelist[128,184)
// [64*1024+1,256*1024] 8*1024byte对⻬ freelist[184,208)
class SizeClass
{
public:
	static inline size_t _RoundUp(size_t bytes, size_t alignNum)
	{
		size_t res;
		if (bytes % alignNum == 0)
		{
			res = bytes;
		}
		else
		{
			res = (bytes / alignNum + 1) * alignNum;
		}
		return res;
	}
	static inline size_t RoundUp(size_t bytes)
	{
		if (bytes <= 128)
		{
			return _RoundUp(bytes, 8);
		}
		else if (bytes <= 1024)
		{
			return _RoundUp(bytes, 16);
		}
		else if (bytes <= 8 * 1024)
		{
			return _RoundUp(bytes, 128);
		}
		else if (bytes <= 64 * 1024)
		{
			return _RoundUp(bytes, 1024);
		}
		else if (bytes <= 256 * 1024)
		{
			return _RoundUp(bytes, 8 * 1024);
		}
		else
		{
			//线程申请空间大于threadcache可以申请到的空间大小
		}
	}
	static inline size_t _Index(size_t size, size_t alignNum)
	{
		size_t res;
		if (size % alignNum == 0)
		{
			res = size / alignNum - 1;
		}
		else
		{
			res = size / alignNum;
		}
		return res;
	}
	static inline size_t Index(size_t size)
	{
		static int group_array[4] = { 16,56,56,56 };
		if (size <= 128)
		{
			return _Index(size, 8);
		}
		else if (size <= 1024)
		{
			return _Index(size, 16) + group_array[0];
		}
		else if (size <= 8 * 1024)
		{
			return _Index(size, 128) + group_array[0] + group_array[1];
		}
		else if (size <= 64 * 1024)
		{
			return _Index(size, 1024) + group_array[0] + group_array[1] + group_array[2];

		}
		else if (size <= 256 * 1024)
		{
			return _Index(size, 8 * 1024) + group_array[0] + group_array[1] + group_array[2] + group_array[3];
		}
		else
		{
			assert(-1);
		}
	}
};
