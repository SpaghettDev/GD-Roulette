#include "LevelInfoLayer.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

class CustomDirector : public CCDirector
{
public:
	CCScene* getPreviousScene()
	{
		return reinterpret_cast<CCScene*>(m_pobScenesStack->objectAtIndex(m_pobScenesStack->count() - 2));
	}
};

namespace LevelInfoLayer
{
	bool __fastcall initHook(gd::LevelInfoLayer* self, void*, gd::GJGameLevel* level)
	{
		if (!init(self, level))
			return false;

		if (RouletteManager.isPlayingRoulette)
		{
			CCNode* normalPercentageNode = nullptr;
			float goalOffset = .0f;

			if (self->m_pLevel->normalPercent < 10)
				goalOffset = 24.f;
			else if (self->m_pLevel->normalPercent <= 100)
				goalOffset = 31.f;

			CCObject* levelInfoLayerObject;
			CCARRAY_FOREACH(self->getChildren(), levelInfoLayerObject)
			{
				auto levelInfoLayerNode = reinterpret_cast<CCNode*>(levelInfoLayerObject);
				if (
					auto levelInfoLayerLabel = dynamic_cast<CCLabelBMFont*>(levelInfoLayerNode);
					levelInfoLayerLabel &&
					strcmp(levelInfoLayerLabel->getString(), CCString::createWithFormat("%d%%", self->m_pLevel->normalPercent)->getCString()) == 0
					) {
					normalPercentageNode = levelInfoLayerNode;
					break;
				}
			};

			if (normalPercentageNode == nullptr) return self;

			auto goalPercentage = CCLabelBMFont::create(CCString::createWithFormat("(%d%%)", static_cast<int>(RouletteManager.levelPercentageGoal))->getCString(), "bigFont.fnt");
			goalPercentage->setPosition({ normalPercentageNode->getPositionX() + goalOffset, normalPercentageNode->getPositionY() });
			goalPercentage->setScale(.4f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(3);
			self->addChild(goalPercentage);

			// the game sometimes just removes the creator's name for some reason
			self->m_pLevel->userName = RouletteManager.levelCreatorName;
		}

		return true;
	}

	void __fastcall onBackHook(gd::LevelInfoLayer* self, void*, CCObject* sender)
	{
		CustomDirector* director = reinterpret_cast<CustomDirector*>(CCDirector::sharedDirector());

		if (RouletteManager.isPlayingRoulette && self->m_pLevel->levelID == RouletteManager.lastLevelID)
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

					self->m_pLevel->userName = RouletteManager.levelCreatorName;
				}
			}
		}

		onBack(self, sender);
	}
}
