#pragma once
#include <matjson.hpp>
#include <array>
#include <atomic>
#include <string>

#include <Geode/Enums.hpp>

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

	inline static const std::map<GJDifficulty, int> m_cDemonDiffToFilter{
		{ GJDifficulty::DemonEasy, 1 },
		{ GJDifficulty::DemonMedium, 2 },
		{ GJDifficulty::Demon, 3 },
		{ GJDifficulty::DemonInsane, 4 },
		{ GJDifficulty::DemonExtreme, 5 }
	};
	// these are only for rated Tiny to XL levels
	inline static const std::array<int, 10> m_cNormalListMaxPage{
		74, // Easy
		173, // Normal
		1031, // Hard
		1308, // Harder
		543, // Insane

		142, // Hard Demon
		187, // Easy Demon
		205, // Medium Demon
		99, // Insane Demon
		90 // Extreme Demon
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
