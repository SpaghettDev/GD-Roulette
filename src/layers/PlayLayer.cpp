#include "../roulette/manager/RouletteManager.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

float previousPosition = .0f, delta = -1.f;

class $modify(GJBaseGameLayer)
{
	void update(float dt)
	{
		if (this->m_player1->getPositionX() != previousPosition)
			delta += dt;

		previousPosition = this->m_player1->getPositionX();

		GJBaseGameLayer::update(dt);
	}
};

class $modify(PlayLayerPause, PlayLayer)
{
	struct Fields
	{
		CCAction* pause_game_action = nullptr;
	};

	void pause()
	{
		GameManager::sharedState()->getPlayLayer()->pauseGame(false);

		CCDirector::sharedDirector()->getRunningScene()->stopAction(m_fields->pause_game_action);
	}

	bool init(GJGameLevel* level, bool p1, bool p2)
	{
		delta = .0f;
		previousPosition = .0f;

		return PlayLayer::init(level, p1, p2);
	}

	void resetLevel()
	{
		delta = .0f;
		previousPosition = .0f;

		PlayLayer::resetLevel();
	}

	void destroyPlayer(PlayerObject* player, GameObject* obj)
	{
#ifdef GEODE_IS_MACOS
		const float currentDelta = delta;

		PlayLayer::destroyPlayer(player, obj);

		const int percentage = m_level->m_normalPercent;
#else
		const int percentage = this->getCurrentPercentInt();
		const float currentDelta = delta;
#endif // GEODE_IS_MACOS
		if (
			g_rouletteManager.isPlaying &&
			this->m_level->m_levelID == g_rouletteManager.gameState.levelID &&
			!this->m_isPracticeMode &&
			percentage >= g_rouletteManager.gameState.levelPercentageGoal
			) {
			if (currentDelta > .2f/* && !this->m_player1->m_isDead*/)
			{
				g_rouletteManager.gameState.hasReachedGoal = true;
				g_rouletteManager.gameState.levelPercentage = percentage;
				g_rouletteManager.gameState.levelPercentageGoal = percentage + 1;
				g_rouletteManager.gameState.numLevels++;

				if (Mod::get()->getSettingValue<bool>("auto-pause"))
				{
					const auto runningScene = CCDirector::sharedDirector()->getRunningScene();

					m_fields->pause_game_action = runningScene->runAction(
						CCSequence::create(
							CCDelayTime::create(1.f),
							CCCallFunc::create(runningScene, callfunc_selector(PlayLayerPause::pause)),
							nullptr
						)
					);
				}
			}
		}

#ifndef GEODE_IS_MACOS
		PlayLayer::destroyPlayer(player, obj);
#endif // !GEODE_IS_MACOS
	}

	void levelComplete()
	{
		if (
			g_rouletteManager.isPlaying &&
			this->m_level->m_levelID == g_rouletteManager.gameState.levelID &&
			!this->m_isPracticeMode
			) {
			g_rouletteManager.gameState.hasReachedGoal = true;
			g_rouletteManager.gameState.levelPercentage = 100;
			g_rouletteManager.gameState.levelPercentageGoal = 100;
			g_rouletteManager.gameState.numLevels++;
		}

		PlayLayer::levelComplete();
	}
};
