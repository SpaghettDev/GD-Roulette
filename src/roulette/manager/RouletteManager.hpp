#pragma once
#include <atomic>
#include <array>

#include <Geode/Geode.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;


struct RouletteManagerStruct
{

private:
	inline static bool hasInitManager = false;

public:
	inline static std::atomic_bool isPlayingRoulette = false;
	inline static std::atomic_bool hasFinishedPreviousLevel = false;

	inline static matjson::Value* difficultyArr; // 6
	inline static matjson::Value* demonDifficultyArr; // 5
	inline static matjson::Value* selectedListArr; // 3

	inline static int lastLevelID = 0;
	inline static int lastLevelPercentage = 0;
	inline static float levelPercentageGoal = 1.f;
	inline static std::string levelCreatorName = "";

	inline static int skipsUsed = 0;

	inline static int numLevels = 0;


	RouletteManagerStruct()
	{
		if (!hasInitManager)
		{
			if (!Mod::get()->hasSavedValue("difficulty-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("difficulty-array", { true, false, false, false, false, false });
			difficultyArr = &(Mod::get()->getSaveContainer().as_object()["difficulty-array"]);

			log::debug("{}", Mod::get()->getSaveContainer().dump(0));

			if (!Mod::get()->hasSavedValue("demon-difficulty-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("demon-difficulty-array", { true, false, false, false, false });
			demonDifficultyArr = &(Mod::get()->getSaveContainer().as_object()["demon-difficulty-array"]);

			if (!Mod::get()->hasSavedValue("selected-list-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("selected-list-array", { true, false, false });
			selectedListArr = &(Mod::get()->getSaveContainer().as_object()["selected-list-array"]);

			hasInitManager = true;
		}
	}

	void reset()
	{
		isPlayingRoulette = false;
		hasFinishedPreviousLevel = false;
		lastLevelID = 0;
		lastLevelPercentage = 0.f;
		levelPercentageGoal = 1.f;
		levelCreatorName = "";
		skipsUsed = 0;
		numLevels = 0;
	}
};

// TODO: inline variables exist...
#ifdef INITIALIZEROULETTEMANAGER
extern RouletteManagerStruct RouletteManager{};
#elif defined(DECLAREROULETTEMANAGER)
extern RouletteManagerStruct RouletteManager;
#endif // !DECLAREROULETTEMANAGER
