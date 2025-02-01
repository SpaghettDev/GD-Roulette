#ifdef GEODE_IS_ANDROID
#include <Geode/modify/AppDelegate.hpp>
#endif

#include "managers/CacheManager.hpp"
#include "listfetcher/ListFetcher.hpp"

$execute
{
	CacheManager::get().addCacheFunction<CMKey::NORMAL_LIST_MAX_PAGES>(
		ListFetcher::normalListCacheFunction
	);
}

#ifdef GEODE_IS_ANDROID
class $modify(RLAppDelegate, AppDelegate)
{
	void applicationDidEnterBackground()
	{
		CacheManager::get().save();
		AppDelegate::applicationDidEnterBackground();
	}

	void trySaveGame(bool p0)
	{
		CacheManager::get().save();
		AppDelegate::trySaveGame(p0);
	}
};
#endif
