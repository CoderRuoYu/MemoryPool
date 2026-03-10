#pragma once
#include "Common.hpp"

class CentralCache
{
public:
	static CentralCache& GetCentralCacheInstance()
	{
		return _centralCache;
	}
	bool IsEmpty(size_t index)
	{
		return _spanList[index].IsEmpty();
	}
private:
	CentralCache() {}
	CentralCache(const CentralCache&) = delete;
	SpanList _spanList[NFreeLists];
	static CentralCache _centralCache;

};
CentralCache CentralCache::_centralCache;
