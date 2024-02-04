#pragma once
#include <atomic>
#include <array>

#include "../layers/RouletteLayer.hpp"
#include "../../utils.hpp"

#include <Geode/Geode.hpp>
#include <matjson/stl_serialize.hpp>

using namespace geode::prelude;


struct RouletteManager
{
private:
	inline static bool hasInitManager = false;

	void verifyArray(matjson::Array& arr)
	{
		if (roulette::utils::getCountOf(arr, true) != 1) {
			for (auto& elem : arr)
				elem = false;
			arr.at(0) = true;
		}
	}

public:
	inline static RouletteLayer* rouletteLayer;

	inline static std::atomic_bool isPlayingRoulette = false;
	inline static bool hasFinishedPreviousLevel = false;

	inline static matjson::Value* difficultyArr; // 6
	inline static matjson::Value* demonDifficultyArr; // 5
	inline static matjson::Value* selectedListArr; // 3

	inline static int lastLevelID = 0;
	inline static int lastLevelPercentage = 0;
	inline static int levelPercentageGoal = 1;
	inline static std::string levelCreatorName = "";

	inline static int skipsUsed = 0;

	inline static int numLevels = 0;

	inline static std::size_t previousDifficulty;


	RouletteManager()
	{
		if (!hasInitManager)
		{
			auto& saveContainer = Mod::get()->getSaveContainer().as_object();

			if (!Mod::get()->hasSavedValue("difficulty-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("difficulty-array", { true, false, false, false, false, false });
			difficultyArr = &(saveContainer["difficulty-array"]);
			verifyArray(difficultyArr->as_array());

			if (!Mod::get()->hasSavedValue("demon-difficulty-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("demon-difficulty-array", { true, false, false, false, false });
			demonDifficultyArr = &(saveContainer["demon-difficulty-array"]);
			verifyArray(demonDifficultyArr->as_array());

			if (!Mod::get()->hasSavedValue("selected-list-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("selected-list-array", { true, false, false });
			selectedListArr = &(saveContainer["selected-list-array"]);
			verifyArray(selectedListArr->as_array());

			previousDifficulty = roulette::utils::getIndexOf(difficultyArr->as_array(), true);

			hasInitManager = true;
		}
	}

	bool getArrayState(matjson::Value* arr, std::size_t idx)
	{
		return arr->as_array().at(idx).as<bool>();
	}

	void reset()
	{
		isPlayingRoulette = false;
		hasFinishedPreviousLevel = false;
		lastLevelID = 0;
		lastLevelPercentage = 0;
		levelPercentageGoal = 1;
		levelCreatorName = "";
		skipsUsed = 0;
		numLevels = 0;
	}
};

inline RouletteManager g_rouletteManager{};
