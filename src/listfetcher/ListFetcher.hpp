#pragma once

#include <matjson.hpp>
#include <atomic>
#include <string>

#include <Geode/Enums.hpp>

#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include <rtrp/responses/ListResponse.hpp>
#include <rtrp/objects/LevelObject.hpp>

#include "SingletonBase.hpp"
#include "constants.hpp"

class ListFetcher : public SingletonBase<ListFetcher>
{
protected:
	ListFetcher();

public:
	using level_pair_t = std::pair<rtrp::objects::LevelObject, rtrp::objects::CreatorObject>;

	void getRandomNormalListLevel(GJDifficulty, geode::Result<level_pair_t>&);
	void getRandomDemonListLevel(geode::Result<level_pair_t>&);
	void getRandomChallengeListLevel(geode::Result<level_pair_t>&);
	void getRandomGDListLevel(int, geode::Result<level_pair_t>&);

	void getLevelInfo(int, geode::Result<level_pair_t>&);

	void setFinishedFetchingCallback(std::function<void()>&& cb) { m_finished_fetching_cb = std::move(cb); }

	static matjson::Value normalListCacheFunction();

	std::atomic_bool is_fetching;

private:
	inline static constexpr std::string_view GJ_SECRET = "Wmfd2893gb7";
	inline static constexpr std::string_view GJ_LEVELS_URL = "https://www.boomlings.com/database/getGJLevels21.php";
	inline static constexpr std::string_view GJ_LISTS_URL = "https://www.boomlings.com/database/getGJLevelLists.php";

	inline static constexpr std::string_view GJ_LEN_QUERY = "0,1,2,3,4";

	inline static constexpr std::string_view DEMONLIST_URL = "https://pointercrate.com/api/v2/demons/listed";
	inline static constexpr std::string_view CHALLENGELIST_URL = "https://challengelist.gd/api/v1/demons";

	inline static std::string getDifficultyQuery(GJDifficulty difficulty)
	{
		if (difficulty < GJDifficulty::Demon)
			return fmt::format("&diff={}", static_cast<int>(difficulty));
		else
			return fmt::format("&diff={}&demonFilter={}", -2, rl::constants::list_fetcher::DEMON_DIFF_TO_FILTER.at(difficulty));
	}

	std::uint64_t m_cached_gd_list_id;
	std::vector<std::string> m_cached_gd_list_level_ids;

	std::function<void()> m_finished_fetching_cb;

	geode::EventListener<geode::utils::web::WebTask> m_main_listener;
	// used when fetching demonlist/challengelist/gdlist
	geode::EventListener<geode::utils::web::WebTask> m_secondary_listener;
};
