#pragma once
#include <atomic>
#include <array>
#include <matjson/stl_serialize.hpp>

#include <Geode/Loader/Mod.hpp>
#include <Geode/Enums.hpp>

#include "../layers/RLRouletteLayer.hpp"
#include "../../utils.hpp"

using namespace geode::prelude;


struct RouletteManager
{
private:
	inline static bool m_hasInitManager = false;

	struct SavedArrayInfo
	{
		const std::size_t size;
		const std::vector<bool> default_value;

		SavedArrayInfo(std::size_t size, const std::vector<bool>& default_value)
			: size(size), default_value(default_value)
		{}
	};

	const std::unordered_map<std::string, SavedArrayInfo> ARRAY_TO_SAI{
		{ "difficulty-array", { 6, { true, false, false, false, false, false } } },
		{ "demon-difficulty-array", { 5, { true, false, false, false, false, false }} },
		{ "selected-list-array", { 4, { true, false, false, false }} }
	};

	void verifyArray(const std::string& name, matjson::Array& arr)
	{
		if (rl::utils::getCountOf(arr, true) != 1)
		{
			for (auto& elem : arr)
				elem = false;
			arr.at(0) = true;
		}

		if (
			auto arraySaveDataInfo = ARRAY_TO_SAI.at(name);
			arraySaveDataInfo.size != arr.size()
		) {
			auto previousIndex = rl::utils::getIndexOf(arr, true);

			Mod::get()->setSavedValue<std::vector<bool>>(name, arraySaveDataInfo.default_value);

			if (previousIndex != -1)
				getFromSaveContainer(name).as_array().at(previousIndex) = true;
		}
	}

public:
	inline static RLRouletteLayer* rouletteLayer;

	inline static std::atomic_bool isPlaying = false;
	inline static std::atomic_bool isPaused = false;
	inline static bool hasFinishedPreviousLevel = false;

	inline static int currentLevelID = 0;
	inline static int currentLevelPercentage = 0;
	inline static int levelPercentageGoal = 1;

	inline static int skipsUsed = 0;

	inline static int numLevels = 0;

	inline static int gdListID = 0;

	inline static GJDifficulty previousDifficulty = GJDifficulty::Easy;
	inline static GJDifficulty previousDemonDifficulty = GJDifficulty::Demon;


	RouletteManager()
	{
		if (!m_hasInitManager)
		{
			auto& saveContainer = Mod::get()->getSaveContainer().as_object();

			if (!Mod::get()->hasSavedValue("difficulty-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("difficulty-array", { true, false, false, false, false, false });

			{
				auto& arr = getFromSaveContainer("difficulty-array").as_array();

				previousDifficulty = getDifficultyFromSaveContainer("difficulty-array");
				previousDemonDifficulty = getDifficultyFromSaveContainer("demon-difficulty-array");
				verifyArray("difficulty-array", arr);
			}


			if (!Mod::get()->hasSavedValue("demon-difficulty-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("demon-difficulty-array", { true, false, false, false, false });
			verifyArray("demon-difficulty-array", saveContainer["demon-difficulty-array"].as_array());


			if (!Mod::get()->hasSavedValue("selected-list-array"))
				Mod::get()->setSavedValue<std::vector<bool>>("selected-list-array", { true, false, false, false });
			verifyArray("selected-list-array", saveContainer["selected-list-array"].as_array());


			if (!Mod::get()->hasSavedValue("gdListID"))
				Mod::get()->setSavedValue<uint64_t>("gdListID", static_cast<uint64_t>(gdListID));
			else
				gdListID = static_cast<int>(Mod::get()->getSavedValue<uint64_t>("gdListID"));

			m_hasInitManager = true;
		}
	}

	// not using getSavedValue because i want a reference and not a copy
	matjson::Value& getFromSaveContainer(const std::string_view key)
	{
		return Mod::get()->getSaveContainer().as_object()[key];
	}

	bool getArrayState(matjson::Value& arr, std::size_t idx)
	{
		return arr.as_array().at(idx).as<bool>();
	}

	GJDifficulty getDifficultyFromSaveContainer(const std::string_view key)
	{
		int idx = rl::utils::getIndexOf(getFromSaveContainer(key).as_array(), true);

		if (key == "demon-difficulty-array")
			return rl::constants::idx_to_demon_diff[idx];
		else
			return static_cast<GJDifficulty>(idx + 1);
	}

	GJDifficulty getDifficultyFromSaveContainer()
	{
		int idx = rl::utils::getIndexOf(getFromSaveContainer("difficulty-array").as_array(), true);

		if (auto difficulty = static_cast<GJDifficulty>(idx + 1); difficulty < GJDifficulty::Demon)
			return difficulty;
		else
		{
			int demonIdx = rl::utils::getIndexOf(getFromSaveContainer("demon-difficulty-array").as_array(), true);
			return rl::constants::idx_to_demon_diff[demonIdx];
		}
	}

	void reset()
	{
		isPlaying = false;
		isPaused = false;
		hasFinishedPreviousLevel = false;
		currentLevelID = 0;
		currentLevelPercentage = 0;
		levelPercentageGoal = 1;
		skipsUsed = 0;
		numLevels = 0;
	}
};

inline RouletteManager g_rouletteManager{};
