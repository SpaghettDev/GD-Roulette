#pragma once

#include <atomic>

#include <Geode/loader/Mod.hpp>
#include <Geode/Enums.hpp>

#include "DataManager.hpp"
#include "GameState.hpp"
#include "../layers/RLRouletteLayer.hpp"
#include "../../utils.hpp"

using namespace geode::prelude;


struct RouletteManager
{
private:
	inline static bool m_hasInitManager = false;

public:
	inline static RLRouletteLayer* rouletteLayer;

	inline static std::atomic_bool isPlaying = false;
	inline static std::atomic_bool isPaused = false;

	inline static GameState gameState{};
	// used visually, not saved.
	inline static int currentPercentageGoal = 1;
	inline static int gdListID = 0;

	inline static GJDifficulty previousDifficulty = GJDifficulty::Easy;
	inline static GJDifficulty previousDemonDifficulty = GJDifficulty::Demon;


	RouletteManager()
	{
		if (!m_hasInitManager)
		{
			static_cast<void>(DataManager::setDefaultSafe<DMArrayKey::DIFFICULTY_ARRAY>());
			static_cast<void>(DataManager::setDefaultSafe<DMArrayKey::DEMON_DIFFICULTY_ARRAY>());
			static_cast<void>(DataManager::setDefaultSafe<DMArrayKey::SELECTED_LIST_ARRAY>());

			gdListID = DataManager::setDefaultSafe<DMMiscKey::GD_LIST_ID>();
			gameState = DataManager::setDefaultSafe<DMMiscKey::SAVE_DATA>();

			if (gameState.levelID != 0)
				currentPercentageGoal = gameState.levelPercentageGoal;

			previousDifficulty = getDifficultyFromSaveContainer<DMArrayKey::DIFFICULTY_ARRAY>();
			previousDemonDifficulty = getDifficultyFromSaveContainer<DMArrayKey::DEMON_DIFFICULTY_ARRAY>();

			m_hasInitManager = true;
		}
	}

	void addExclamationMark(CCMenuItemSpriteExtra* button = nullptr)
	{
		auto exclamationSprite = CCSprite::createWithSpriteFrameName(
			"exMark_001.png"
		);
		exclamationSprite->setScale(.6f);
		exclamationSprite->setID("exclamation-mark");
		(button
			? button
			: rouletteLayer
				->getParent()
				->getChildByType<CreatorLayer>(0)
				->getChildByID("center-left-menu"_spr)
				->getChildByID("roulette-button"_spr)
		)->addChild(exclamationSprite);

		exclamationSprite->setLayoutOptions(
			AnchorLayoutOptions::create()
			->setAnchor(Anchor::TopRight)
			->setOffset({ -5.f, -8.f })
		);
	}

	template <DMArrayKey key>
	GJDifficulty getDifficultyFromSaveContainer()
	{
		int idx = rl::utils::getIndexOf(DataManager::get<key>().asVector(), true);

		if constexpr (key == DMArrayKey::DEMON_DIFFICULTY_ARRAY)
			return rl::constants::idx_to_demon_diff[idx];
		else
			return static_cast<GJDifficulty>(idx + 1);
	}

	GJDifficulty getDifficultyFromSaveContainer()
	{
		int idx = rl::utils::getIndexOf(DataManager::get<DMArrayKey::DIFFICULTY_ARRAY>().asVector(), true);

		if (auto difficulty = static_cast<GJDifficulty>(idx + 1); difficulty < GJDifficulty::Demon)
			return difficulty;
		else
		{
			int demonIdx = rl::utils::getIndexOf(DataManager::get<DMArrayKey::DEMON_DIFFICULTY_ARRAY>().asVector(), true);
			return rl::constants::idx_to_demon_diff[demonIdx];
		}
	}

	void saveState()
	{
		DataManager::set<DMMiscKey::SAVE_DATA>(gameState);
	}

	void resetState()
	{
		gameState.levelID = 0;
		gameState.levelPercentage = 0;
		gameState.levelPercentageGoal = 0;
		gameState.numLevels = 0;
		gameState.skipsUsed = 0;
		gameState.hasReachedGoal = false;
	}

	void reset()
	{
		resetState();
		saveState();

		isPlaying = false;
		isPaused = false;
		currentPercentageGoal = 1;
	}
};

inline RouletteManager g_rouletteManager{};
