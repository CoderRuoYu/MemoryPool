#pragma once
#include "Common.hpp"
#include "PageCache.hpp"

class CentralCache
{
public:
	void RealseListToSpan(void* start,size_t size)
	{
		//对一个桶进行操作的时候要加锁
		size_t index = SizeClass::Index(size);
		_spanList[index]._mtx.lock();
		while (start)
		{

			void* next = NextObj(start);
			Span* tmp = PageCache::GetPageCacheInstance().ObjToSpan(start);
			NextObj(start) = tmp->_freeList;
			tmp->_freeList = start;
			tmp->_useCount--;
			
			if (tmp->_useCount == 0)
			{
				_spanList[index].Erase(tmp);
				tmp->_freeList = nullptr;
				tmp->_next = nullptr;
				tmp->_prev = nullptr;
				_spanList[index]._mtx.unlock();
				//PageCache::
				PageCache::GetPageCacheInstance()._mtxPage.lock();
				PageCache::GetPageCacheInstance().ReleaseSpanToPageCache(tmp);
				PageCache::GetPageCacheInstance()._mtxPage.unlock();
				_spanList[index]._mtx.lock();
			}
			start = next;
			

		}
		_spanList[index]._mtx.unlock();

	}
	static CentralCache& GetCentralCacheInstance()
	{
		return _centralCache;
	}
	Span* GetOneSpan(SpanList& list, size_t size)
	{
		Span* begin = list.Begin();
		Span* end = list.End();
		while (begin != end)
		{
			if (begin->_freeList != nullptr)
			{
				return begin;
			}
			else
			{
				begin = begin->_next;
			}
		}
		PageCache::GetPageCacheInstance()._mtxPage.lock();
		Span* newSpan = PageCache::GetPageCacheInstance().NewSpan(SizeClass::SizeToPageNum(size));
		PageCache::GetPageCacheInstance()._mtxPage.unlock();
		//对Span进行切分
		newSpan->_freeList = (void*)(newSpan->_pageid << PageShift);
		char* start = (char*)newSpan->_freeList;
		char* tail = (char*)((newSpan->_pageid + newSpan->_n) << PageShift);
		
		while (start + size != tail)
		{
			char* tmp = start + size;
			NextObj(start) = tmp;
			start += size;
		}
		NextObj(start) = nullptr;
		
		list.PushFront(newSpan);
		return newSpan;
	}

	size_t FetchRangeObj(void*& begin, void*& end, size_t batchNum, size_t size)
	{
		size_t index = SizeClass::Index(size);

		_spanList[index]._mtx.lock();
		Span* span = GetOneSpan(_spanList[index], size);
		begin = end = span->_freeList;
		size_t actualNum = 1;
		for (int i = 0; i < batchNum; i++)
		{
			if (NextObj(end) != nullptr)
			{
				end = NextObj(end);
				actualNum++;
			}
			else
			{
				break;
			}
		}
		span->_freeList = NextObj(end);
		span->_useCount += actualNum;
		NextObj(end) = nullptr;
		_spanList[index]._mtx.unlock();
		return actualNum;
	}

private:
	CentralCache() {}
	CentralCache(const CentralCache&) = delete;
	static CentralCache _centralCache;

	SpanList _spanList[NFreeLists];
};
CentralCache CentralCache::_centralCache;
