#pragma once
#include <iostream>
#include <assert.h>
#include <thread>
#include <mutex>
#include <algorithm>
using std::cout;
using std::endl;

static const int PageNum = 128;
static const int NFreeLists = 208;
static const int MaxBytes = 256 * 1024;
#ifdef _WIN64
typedef unsigned long long PAGEID;
#elif _WIN32
typedef size_t PAGEID;
#endif
void*& NextObj(void* tmp)
{
	assert(tmp != nullptr);
	return *(void**)tmp;
	
}
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
	void PushRangeObj(void* begin, void* end)
	{
		assert(begin != nullptr);
		assert(end != nullptr);
		NextObj(end) = _freeList;
		_freeList = begin;
	}
	bool IsEmpty()
	{
		return _freeList == nullptr;
	}
	size_t& GetMaxSize()
	{
		return _maxSize;
	}
private:
	void* _freeList = nullptr;
	size_t _maxSize = 1;
};

struct Span
{
	//页号
	PAGEID _pageid = 0;

	Span* _next = nullptr;
	Span* _prev = nullptr;

	void* _freeList = nullptr;

	bool _isUse = false;
	size_t _useCount = 0;


};
//带头节点的双向循环链表
class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}
	void Insert(struct Span* pos, struct Span* newSpan)
	{
		assert(pos);
		assert(newSpan);
		Span* prev = pos->_prev;
		prev->_next = newSpan;
		newSpan->_prev = prev;
		newSpan->_next = pos;
		pos->_prev = newSpan;
	}
	void Erase(struct Span* pos)
	{
		assert(pos);
		assert(_head != nullptr);
		Span* prev = pos->_prev;
		Span* next = pos->_next;
		prev->_next = next;
		next->_prev = prev;
	}
	Span* Begin()
	{
		return _head->_next;
	}
	Span* End()
	{
		return _head;
	}
public:
	std::mutex _mtx;
private:
	struct Span* _head;
	
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
			//线程申请空间大于threadcache可以申请到的空间大小 有待填充？？？？
			assert(false);
			exit(1);

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
			//线程申请空间大小，大于thread cache可以分配空间大小如何处理？？ 有待填充
			assert(-1);
		}
	}
	static inline size_t SizeToBatchNum(size_t size)
	{
		size_t num = MaxBytes / size;
		if (num < 2)
			num = 2;
		if (num > 512)
			num = 512;
		return num;
	}
};
