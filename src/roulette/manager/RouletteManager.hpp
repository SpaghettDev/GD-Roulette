#pragma once
#include "../../pch.hpp"
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

	inline static std::array<bool, 6> difficultyArr;
	inline static std::array<bool, 5> demonDifficultyArr;
	inline static WriteInvoking<std::array<bool, 3>> selectedListArr;

	inline static int lastLevelID = 0;
	inline static int lastLevelPercentage = 0;
	inline static float levelPercentageGoal = 1.f;
	inline static std::string levelCreatorName = "";

	inline static int skipsCount = 0;
	inline static WriteInvoking<int> maxSkips = 3;

	inline static int numLevels = 0;

	inline static bool isJsonCorrupted = false;

	RouletteManagerStruct()
	{
		if (!hasInitManager)
		{
			difficultyArr.fill(false);
			demonDifficultyArr.fill(false);
			selectedListArr.fill(false);

			difficultyArr[0] = true;
			demonDifficultyArr[0] = true;
			selectedListArr[0] = true;

			hasInitManager = true;
		}
	}
};

#ifdef INITIALIZEROULETTEMANAGER
extern RouletteManagerStruct RouletteManager{};
#elif defined(DECLAREROULETTEMANAGER)
extern RouletteManagerStruct RouletteManager;
#endif // !DECLAREROULETTEMANAGER
