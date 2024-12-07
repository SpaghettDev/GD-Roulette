#pragma once

#include <matjson.hpp>
#include <array>
#include <atomic>
#include <string>

#include <Geode/Enums.hpp>

#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include <rtrp/responses/ListResponse.hpp>
#include <rtrp/objects/LevelObject.hpp>

class ListFetcher
{
private:
	inline static constexpr std::string_view GJ_SECRET = "Wmfd2893gb7";
	inline static constexpr std::string_view GJ_LEVELS_URL = "https://www.boomlings.com/database/getGJLevels21.php";
	inline static constexpr std::string_view GJ_LISTS_URL = "https://www.boomlings.com/database/getGJLevelLists.php";

	inline static constexpr std::string_view GJ_LEN_QUERY = "0,1,2,3,4";


	inline static constexpr std::string_view DEMONLIST_URL = "https://pointercrate.com/api/v2/demons/listed";
	inline static constexpr std::string_view CHALLENGELIST_URL = "https://challengelist.gd/api/v1/demons";

	// TODO: move to rl::constants
	inline static const std::map<GJDifficulty, int> m_cDemonDiffToFilter{
		{ static_cast<GJDifficulty>(-2), 0 },
		{ GJDifficulty::DemonEasy, 1 },
		{ GJDifficulty::DemonMedium, 2 },
		{ GJDifficulty::Demon, 3 },
		{ GJDifficulty::DemonInsane, 4 },
		{ GJDifficulty::DemonExtreme, 5 }
	};
	// these are only for rated Tiny to XL levels
	// TODO: fetch dynamically
	inline static const std::array<int, 11> m_cNormalListMaxPage{
		84, // Easy
		189, // Normal
		1156, // Hard
		1497, // Harder
		634, // Insane

		803, // Demon

		148, // Hard Demon
		210, // Easy Demon
		229, // Medium Demon
		109, // Insane Demon
		108 // Extreme Demon
	};
	inline static const int m_cDemonListMaxPage = 490;

	int m_cachedGDListID;
	std::vector<std::string> m_cachedGDListLevelIDs;

	inline static std::string getDifficultyQuery(GJDifficulty difficulty)
	{
		if (difficulty < GJDifficulty::Demon)
			return fmt::format("&diff={}", static_cast<int>(difficulty));
		else
			return fmt::format("&diff={}&demonFilter={}", -2, m_cDemonDiffToFilter.at(difficulty));
	}

	geode::EventListener<geode::utils::web::WebTask> m_listener;
	// used when fetching demonlist/challengelist/gdlist
	geode::EventListener<geode::utils::web::WebTask> m_listener2;

public:
	using level_pair_t = std::pair<rtrp::objects::LevelObject, rtrp::objects::CreatorObject>;
	std::atomic_bool is_fetching;

	void init();

	void getRandomNormalListLevel(GJDifficulty, level_pair_t&, std::string&);
	void getRandomDemonListLevel(level_pair_t&, std::string&);
	void getRandomChallengeListLevel(level_pair_t&, std::string&);
	void getRandomGDListLevel(int, level_pair_t&, std::string&);

	void getLevelInfo(int, level_pair_t&, std::string&);
};
