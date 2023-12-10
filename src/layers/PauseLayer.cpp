#include "PauseLayer.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

namespace PauseLayer
{
	gd::PauseLayer* __fastcall createHook(bool p0, void*)
	{
		auto self = create(p0);

		if (RouletteManager.isPlayingRoulette)
		{
			const gd::PlayLayer* playLayer = gd::GameManager::sharedState()->getPlayLayer();

			CCNode* normalPercentageNode = nullptr;
			float goalOffset = 24.f;

			if (playLayer->m_level->normalPercent < 10)
				goalOffset = 28.f;
			else if (playLayer->m_level->normalPercent <= 100)
				goalOffset = 40.f;

			CCObject* pauseLayerObject;
			CCARRAY_FOREACH(self->getChildren(), pauseLayerObject)
			{
				auto pauseLayerNode = reinterpret_cast<CCNode*>(pauseLayerObject);
				if (
					auto levelInfoLayerLabel = dynamic_cast<CCLabelBMFont*>(pauseLayerNode);
					levelInfoLayerLabel &&
					strcmp(levelInfoLayerLabel->getString(), CCString::createWithFormat("%d%%", playLayer->m_level->normalPercent)->getCString()) == 0
					) {
					normalPercentageNode = pauseLayerNode;
					break;
				}
			};

			if (normalPercentageNode == nullptr) return self;

			auto goalPercentage = CCLabelBMFont::create(
				CCString::createWithFormat("(%d%%)", static_cast<int>(RouletteManager.levelPercentageGoal))->getCString(),
				"bigFont.fnt"
			);
			goalPercentage->setPosition({ normalPercentageNode->getPositionX() + goalOffset, normalPercentageNode->getPositionY() });
			goalPercentage->setScale(.5f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(4);
			self->addChild(goalPercentage);
		}

		return self;
	}
}
