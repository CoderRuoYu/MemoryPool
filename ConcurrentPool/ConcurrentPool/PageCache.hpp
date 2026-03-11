#pragma once
#include "Common.hpp"
class PageCache
{
public:
	static PageCache& GetPageCacheInstance()
	{
		return _pagecache;
	}


private:
	PageCache() {};
	PageCache(const PageCache&) = delete;
	static PageCache _pagecache;
	SpanList _spanList[PageNum + 1];
};
