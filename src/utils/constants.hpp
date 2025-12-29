#pragma once

#include <map>
#include <string_view>

#include <Geode/Enums.hpp>

namespace rl
{
	namespace constants
	{
		namespace sprites
		{
			inline const std::map<GJDifficulty, std::string_view> DIFFICULTY_TO_SPRITE{
				{ static_cast<GJDifficulty>(-2), "difficulty_06_btn_001.png" },
				{ static_cast<GJDifficulty>(-1), "difficulty_00_btn_001.png" },
				{ GJDifficulty::Auto, "difficulty_auto_btn_001.png" },
				{ GJDifficulty::Easy, "difficulty_01_btn_001.png" },
				{ GJDifficulty::Normal, "difficulty_02_btn_001.png" },
				{ GJDifficulty::Hard, "difficulty_03_btn_001.png" },
				{ GJDifficulty::Harder, "difficulty_04_btn_001.png" },
				{ GJDifficulty::Insane, "difficulty_05_btn_001.png" },
				{ GJDifficulty::Demon, "difficulty_06_btn2_001.png" },
				{ GJDifficulty::DemonEasy, "difficulty_07_btn2_001.png" },
				{ GJDifficulty::DemonMedium, "difficulty_08_btn2_001.png" },
				{ GJDifficulty::DemonInsane, "difficulty_09_btn2_001.png" },
				{ GJDifficulty::DemonExtreme, "difficulty_10_btn2_001.png" }
			};

			inline const std::map<GJFeatureState, std::string_view> FEATURE_STATE_TO_SPRITE{
				{ GJFeatureState::Featured, "GJ_featuredCoin_001.png" },
				{ GJFeatureState::Epic, "GJ_epicCoin_001.png" },
				{ GJFeatureState::Legendary, "GJ_epicCoin2_001.png" },
				{ GJFeatureState::Mythic, "GJ_epicCoin3_001.png" },
			};
		}

		namespace save_data
		{
			inline const std::array<GJDifficulty, 6> IDX_TO_DIFF{
				GJDifficulty::Easy,
				GJDifficulty::Normal,
				GJDifficulty::Hard,
				GJDifficulty::Harder,
				GJDifficulty::Insane,
				GJDifficulty::Demon
			};
	
			inline const std::map<GJDifficulty, int> DIFF_TO_IDX{
				{ GJDifficulty::Easy, 0 },
				{ GJDifficulty::Normal, 1 },
				{ GJDifficulty::Hard, 2 },
				{ GJDifficulty::Harder, 3 },
				{ GJDifficulty::Insane, 4 },
				{ GJDifficulty::Demon, 5 }
			};
	
			inline const std::array<GJDifficulty, 6> IDX_TO_DEMON_DIFF{
				static_cast<GJDifficulty>(-2),
				GJDifficulty::DemonEasy,
				GJDifficulty::DemonMedium,
				GJDifficulty::Demon,
				GJDifficulty::DemonInsane,
				GJDifficulty::DemonExtreme
			};
	
			inline const std::map<GJDifficulty, int> DEMON_DIFF_TO_IDX{
				{ static_cast<GJDifficulty>(-2), 0 },
				{ GJDifficulty::DemonEasy, 1 },
				{ GJDifficulty::DemonMedium, 2 },
				{ GJDifficulty::Demon, 3 },
				{ GJDifficulty::DemonInsane, 4 },
				{ GJDifficulty::DemonExtreme, 5 }
			};
		}

		namespace list_fetcher
		{
			inline static const std::map<GJDifficulty, std::uint8_t> DEMON_DIFF_TO_FILTER{
				{ static_cast<GJDifficulty>(-2), 0 },
				{ GJDifficulty::DemonEasy, 1 },
				{ GJDifficulty::DemonMedium, 2 },
				{ GJDifficulty::Demon, 3 },
				{ GJDifficulty::DemonInsane, 4 },
				{ GJDifficulty::DemonExtreme, 5 }
			};
	
			inline static const std::uint16_t DEMON_LIST_MAX_PAGE = 542;
			inline static const std::uint16_t CHALLENGE_LIST_MAX_PAGE = 305;
		}
	}
}
