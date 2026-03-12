#pragma once
#include "Common.hpp"
#include "PageCache.hpp"

class CentralCache
{
public:

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
