#include "managers/CacheManager.hpp"
#include "listfetcher/ListFetcher.hpp"

$execute
{
	CacheManager::get().addCacheFunction<CMKey::NORMAL_LIST_MAX_PAGES>(
		ListFetcher::normalListCacheFunction
	);
}
