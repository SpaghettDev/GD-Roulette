#include "../roulette/manager/RouletteManager.hpp"

#include <Geode/Geode.hpp>
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

	// thank you alk <3
	float currentPercent()
	{
		float percent;

		if (m_level->m_timestamp > 0)
			percent = static_cast<float>(m_gameState.m_unk1f8) / m_level->m_timestamp * 100.f;
		else
			percent = m_player1->getPosition().x / m_levelLength * 100.f;

		if (percent >= 100.f)
			return 100.f;
		else if (percent <= 0.f)
			return 0.f;
		else
			return percent;
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
		if (
			const int percentage = static_cast<int>(currentPercent());
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

		PlayLayer::destroyPlayer(player, obj);
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
