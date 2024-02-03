#define DECLAREROULETTEMANAGER
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
			const int percentage = this->getCurrentPercentInt();
			RouletteManager.isPlayingRoulette &&
			this->m_level->m_levelID == RouletteManager.lastLevelID &&
			!this->m_isPracticeMode &&
			percentage >= RouletteManager.levelPercentageGoal
			) {
			if (delta > .2f && !this->m_player1->m_isDead)
			{
				RouletteManager.hasFinishedPreviousLevel = true;
				RouletteManager.lastLevelPercentage = percentage;
				RouletteManager.levelPercentageGoal = RouletteManager.lastLevelPercentage + 1.f;
				RouletteManager.numLevels++;

				const auto runningScene = CCDirector::sharedDirector()->getRunningScene();
				m_fields->pauseGameAction = runningScene->runAction(
					CCSequence::create(
						CCDelayTime::create(1.f), CCCallFunc::create(runningScene, callfunc_selector(PlayLayerPause::pause)), nullptr
					)
				);
			}
		}

		PlayLayer::destroyPlayer(player, obj);
	}

	// TODO: correct bindings (GameSoundManager*)
	void levelComplete()
	{
		if (
			RouletteManager.isPlayingRoulette &&
			this->m_level->m_levelID == RouletteManager.lastLevelID &&
			!this->m_isPracticeMode
			) {
			RouletteManager.hasFinishedPreviousLevel = true;
			RouletteManager.lastLevelPercentage = 100;
			RouletteManager.levelPercentageGoal = 100;
			RouletteManager.numLevels++;
		}

		PlayLayer::levelComplete();
	}
};
