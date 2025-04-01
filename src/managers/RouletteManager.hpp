#pragma once

#include <atomic>

#include <Geode/Enums.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

#include "SingletonBase.hpp"

#include "GameState.hpp"
#include "GameTimer.hpp"

class RLRouletteLayer;


struct RouletteManager : public SingletonBase<RouletteManager>
{
protected:
	RouletteManager();

public:
	static void addExclamationMark(CCMenuItemSpriteExtra*);

	void saveState();
	void reset();

public:
	RLRouletteLayer* rouletteLayer;

	std::atomic_bool isPlaying = false;
	std::atomic_bool isPaused = false;

	GameState gameState{};
	GameTimer gameTimer{};
	// used visually, not saved.
	int currentPercentageGoal = 1;
	int gdListID = 0;

	GJDifficulty previousDifficulty = GJDifficulty::Easy;
	GJDifficulty previousDemonDifficulty = GJDifficulty::Demon;

	bool hasEnteredPlayLayer = false;

	bool hasFinishedARound = false;
};
