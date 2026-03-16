#pragma once
#include "Common.hpp"
#include <windows.h>
void* SystemAlloc(size_t pageNums)
{
	void* ptr = VirtualAlloc(
		NULL,
		pageNums << PageShift,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE
	);
	return ptr;
}
class PageCache
{
public:
	static PageCache& GetPageCacheInstance()
	{
		return _pagecache;
	}
	Span* NewSpan(size_t pageNums)
	{
		assert(pageNums > 0 && pageNums <= Npages);
		if (!_spanList[pageNums].IsEmpty())
		{
			return _spanList[pageNums].PopFront();
		}
		for (int i = pageNums + 1; i < Npages; i++)
		{
			if (_spanList[i].IsEmpty())
			{
				continue;
			}
			else
			{
				Span* tmp = _spanList[i].PopFront();
				Span* cur = new Span;

				cur->_pageid = tmp->_pageid;
				tmp->_pageid += pageNums;

				cur->_n = pageNums;
				tmp->_n -= pageNums;

				_spanList[tmp->_n].PushFront(tmp);
				return cur;
			}
		}
		//到这一步，说明整个链表spanlist中都没有span
		Span* newSpan = new Span;
		void* mem = (char*)SystemAlloc(Npages - 1);
		newSpan->_n = Npages - 1;
		newSpan->_pageid = (PAGEID)mem >> PageShift;
		_spanList[Npages - 1].PushFront(newSpan);
		return NewSpan(pageNums);
	}
public:
	std::mutex _mtxPage;
private:
	PageCache() {};
	PageCache(const PageCache&) = delete;
	static PageCache _pagecache;
	SpanList _spanList[Npages];
	
};
