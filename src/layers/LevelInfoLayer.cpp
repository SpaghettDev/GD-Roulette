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
			CCPoint normalPercentagePos = { 284.5f, 136.f };
			float goalOffset = .0f;

			if (self->m_pLevel->normalPercent < 10)
				goalOffset = 24.f;
			else if (self->m_pLevel->normalPercent <= 100)
				goalOffset = 31.f;

			CCObject* levelInfoLayerObject;
			CCARRAY_FOREACH(self->getChildren(), levelInfoLayerObject)
			{
				CCNode* levelInfoLayerNode = reinterpret_cast<CCNode*>(levelInfoLayerObject);
				if (levelInfoLayerNode->getPositionX() == 284.5f && levelInfoLayerNode->getPositionY() == 136.f)
				{
					normalPercentageNode = levelInfoLayerNode;
					break;
				}
			};

			if (normalPercentageNode != nullptr)
				normalPercentagePos = normalPercentageNode->getPosition();

			auto goalPercentage = CCLabelBMFont::create(CCString::createWithFormat("(%d%%)", static_cast<int>(RouletteManager.levelPercentageGoal))->getCString(), "bigFont.fnt");
			goalPercentage->setPosition({ normalPercentagePos.x + goalOffset, normalPercentagePos.y });
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

		if (!RouletteManager.isPlayingRoulette || self->m_pLevel->levelID != RouletteManager.lastLevelID)
			onBack(self, sender);
		else
		{
			if (
				CCScene* prevScene = director->getPreviousScene();
				prevScene->getChildrenCount() == 2
				) {
				if (
					std::string_view name = (typeid(*prevScene->getChildren()->objectAtIndex(1)).name() + 6);
					name == "RouletteLayer"
					) {
					CCLayer* rouletteLayer = reinterpret_cast<CCLayer*>(
						director->getPreviousScene()->getChildren()->objectAtIndex(1)
					);

					CCMenu* rouletteMenu = reinterpret_cast<CCMenu*>(
						reinterpret_cast<CCLayer*>(
							rouletteLayer->getChildren()->objectAtIndex(0)
						)->getChildren()->objectAtIndex(1)
					);

					reinterpret_cast<CCLabelBMFont*>(
						rouletteMenu->getChildByTag(117)
					)->setString(CCString::createWithFormat("%d%%", RouletteManager.lastLevelPercentage)->getCString());
				}
			}

			onBack(self, sender);
		}
	}
}
