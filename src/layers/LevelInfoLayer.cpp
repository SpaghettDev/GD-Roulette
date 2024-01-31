#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class CustomDirector : public CCDirector
{
public:
	CCScene* getPreviousScene()
	{
		return reinterpret_cast<CCScene*>(m_pobScenesStack->objectAtIndex(m_pobScenesStack->count() - 2));
	}
};

class $modify(LevelInfoLayer)
{
	bool init(GJGameLevel* level, bool p1)
	{
		if (!LevelInfoLayer::init(level, p1)) return false;

		if (RouletteManager.isPlayingRoulette)
		{
			CCNode* normalPercentageNode = nullptr;
			float goalOffset = .0f;

			if (this->m_level->m_normalPercent < 10)
				goalOffset = 24.f;
			else if (this->m_level->m_normalPercent <= 100)
				goalOffset = 31.f;

			CCObject* levelInfoLayerObject;
			CCARRAY_FOREACH(this->getChildren(), levelInfoLayerObject)
			{
				auto levelInfoLayerNode = reinterpret_cast<CCNode*>(levelInfoLayerObject);
				if (
					auto levelInfoLayerLabel = dynamic_cast<CCLabelBMFont*>(levelInfoLayerNode);
					levelInfoLayerLabel &&
					strcmp(levelInfoLayerLabel->getString(), CCString::createWithFormat("%d%%", this->m_level->m_normalPercent)->getCString()) == 0
					) {
					normalPercentageNode = levelInfoLayerNode;
					break;
				}
			};

			if (normalPercentageNode == nullptr) return this;

			auto goalPercentage = CCLabelBMFont::create(CCString::createWithFormat("(%d%%)", static_cast<int>(RouletteManager.levelPercentageGoal))->getCString(), "bigFont.fnt");
			goalPercentage->setPosition({ normalPercentageNode->getPositionX() + goalOffset, normalPercentageNode->getPositionY() });
			goalPercentage->setScale(.4f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(3);
			this->addChild(goalPercentage);
		}

		return true;
	}

	void onBack(CCObject* sender)
	{
		CustomDirector* director = reinterpret_cast<CustomDirector*>(CCDirector::sharedDirector());

		if (RouletteManager.isPlayingRoulette && this->m_level->m_levelID == RouletteManager.lastLevelID)
		{
			if (
				CCScene* prevScene = director->getPreviousScene();
				prevScene->getChildrenCount() == 2
				) {
				if (
					std::string_view prevLayerName = (typeid(*prevScene->getChildren()->objectAtIndex(1)).name() + 6);
					prevLayerName == "RouletteLayer"
					) {
					RouletteLayer* rouletteLayer = reinterpret_cast<RouletteLayer*>(
						director->getPreviousScene()->getChildren()->objectAtIndex(1)
					);

					reinterpret_cast<CCLabelBMFont*>(
						rouletteLayer->m_pPlayingMenu->getChildByTag(20)
					)->setString(CCString::createWithFormat("%d%%", RouletteManager.lastLevelPercentage)->getCString());

					this->m_level->m_creatorName = RouletteManager.levelCreatorName;
				}
			}
		}

		LevelInfoLayer::onBack(sender);
	}
};
