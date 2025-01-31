#include "RouletteManager.hpp"

#include <Geode/modify/EndLevelLayer.hpp>

class $modify(RLEndLevelLayer, EndLevelLayer)
{
	void customSetup()
	{
		EndLevelLayer::customSetup();

		if (!RouletteManager::get().isPlaying || !RouletteManager::get().gameState.hasReachedGoal)
			return;

		auto levelCompleteSprite = this->m_mainLayer->getChildByID("level-complete-text");
		levelCompleteSprite->setVisible(false);

		auto goalReachedSprite = cocos2d::CCSprite::create("RL_goalReached_001.png"_spr);
		goalReachedSprite->setScale(.95f);
		goalReachedSprite->setPosition(levelCompleteSprite->getPosition());
		goalReachedSprite->setID("goal-reached-sprite"_spr);
		this->m_mainLayer->addChild(goalReachedSprite);
	}
};
