#pragma once
#include <atomic>
#include <array>
#include "../../json_manager/WriteInvokingType.hpp"

struct RouletteManagerStruct
{
private:
	inline static bool hasInitManager = false;

public:
	inline static std::atomic_bool isPlayingRoulette = false;
	inline static std::atomic_bool hasFinishedPreviousLevel = false;

	inline static bool showOptionsSprite = true;
	inline static bool rouletteResourcesFound = false;

	inline static std::array<bool, 6> difficultyArr{};
	inline static std::array<bool, 5> demonDifficultyArr{};
	inline static std::array<bool, 3> selectedListArr;

	inline static int lastLevelID = 0;
	inline static int lastLevelPercentage = 0;
	inline static float levelPercentageGoal = 1.f;
	inline static std::string levelCreatorName = "";

	inline static int skipsUsed = 0;
	inline static int maxSkips;

	inline static int numLevels = 0;

	inline static bool isJsonCorrupted = false;

	RouletteManagerStruct()
	{
		if (!hasInitManager)
		{
			difficultyArr.fill(false);
			demonDifficultyArr.fill(false);

			difficultyArr[0] = true;
			demonDifficultyArr[0] = true;

			hasInitManager = true;
		}
	}

	void reset()
	{
		isPlayingRoulette = false;
		hasFinishedPreviousLevel = false;
		lastLevelID = 0;
		lastLevelPercentage = 0;
		levelPercentageGoal = 1.f;
		levelCreatorName = "";
		skipsUsed = 0;
		numLevels = 0;
	}
};

#ifdef INITIALIZEROULETTEMANAGER
extern RouletteManagerStruct RouletteManager{};
#elif defined(DECLAREROULETTEMANAGER)
extern RouletteManagerStruct RouletteManager;
#endif // !DECLAREROULETTEMANAGER
