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
	CCAction* pauseGameAction = nullptr;

	void pause()
	{
		GameManager::sharedState()->getPlayLayer()->pauseGame(false);

		CCDirector::sharedDirector()->getRunningScene()->stopAction(m_fields->pauseGameAction);
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
		PlayLayer::destroyPlayer(player, obj);

		const int percentage = m_level->m_normalPercent;
#else
		const int percentage = this->getCurrentPercentInt();
#endif // GEODE_IS_MACOS
		if (
			g_rouletteManager.isPlaying &&
			this->m_level->m_levelID == g_rouletteManager.currentLevelID &&
			!this->m_isPracticeMode &&
			percentage >= g_rouletteManager.levelPercentageGoal
			) {
			if (delta > .2f/* && !this->m_player1->m_isDead*/)
			{
				g_rouletteManager.hasFinishedPreviousLevel = true;
				g_rouletteManager.currentLevelPercentage = percentage;
				g_rouletteManager.levelPercentageGoal = percentage + 1;
				g_rouletteManager.numLevels++;

				if (Mod::get()->getSettingValue<bool>("auto-pause"))
				{
					const auto runningScene = CCDirector::sharedDirector()->getRunningScene();

					m_fields->pauseGameAction = runningScene->runAction(
						CCSequence::create(
							CCDelayTime::create(1.f), CCCallFunc::create(runningScene, callfunc_selector(PlayLayerPause::pause)), nullptr
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
			this->m_level->m_levelID == g_rouletteManager.currentLevelID &&
			!this->m_isPracticeMode
			) {
			g_rouletteManager.hasFinishedPreviousLevel = true;
			g_rouletteManager.currentLevelPercentage = 100;
			g_rouletteManager.levelPercentageGoal = 100;
			g_rouletteManager.numLevels++;
		}

		PlayLayer::levelComplete();
	}
};
