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

	// inline static matjson::Value* difficultyArr; // 6
	// inline static matjson::Value* demonDifficultyArr; // 5
	// inline static matjson::Value* selectedListArr; // 3

	inline static int currentLevelID = 0;
	inline static int currentLevelPercentage = 0;
	inline static int levelPercentageGoal = 1;

	inline static int skipsUsed = 0;

	inline static int numLevels = 0;

	inline static std::size_t previousDifficulty;


	RouletteManager()
	{
		if (!hasInitManager)
		{
			auto& saveContainer = Mod::get()->getSaveContainer().as_object();

			if (!Mod::get()->hasSavedValue("difficulty-array"))
			{
				Mod::get()->setSavedValue<std::vector<bool>>("difficulty-array", { true, false, false, false, false, false });
				previousDifficulty = 0;
			}
			else
			{
				auto& arr = getFromSaveContainer("difficulty-array").as_array();

				previousDifficulty = roulette::utils::getIndexOf(arr, true);
				verifyArray(arr);
			}

			if (!Mod::get()->hasSavedValue("demon-difficulty-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("demon-difficulty-array", { true, false, false, false, false });
			else
				verifyArray(saveContainer["demon-difficulty-array"].as_array());

			if (!Mod::get()->hasSavedValue("selected-list-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("selected-list-array", { true, false, false });
			else
				verifyArray(saveContainer["selected-list-array"].as_array());

			hasInitManager = true;
		}
	}

	// not using getSavedValue because i want a reference and not a copy
	matjson::Value& getFromSaveContainer(std::string_view const key)
	{
		return Mod::get()->getSaveContainer().as_object()[key];
	}

	bool getArrayState(matjson::Value& arr, std::size_t idx)
	{
		return arr.as_array().at(idx).as<bool>();
	}

	void reset()
	{
		isPlayingRoulette = false;
		hasFinishedPreviousLevel = false;
		currentLevelID = 0;
		currentLevelPercentage = 0;
		levelPercentageGoal = 1;
		skipsUsed = 0;
		numLevels = 0;
	}
};

inline RouletteManager g_rouletteManager{};
