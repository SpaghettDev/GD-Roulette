#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class $modify(PauseLayer)
{
	PauseLayer* create(bool p0)
	{
		auto self = PauseLayer::create(p0);

		if (RouletteManager.isPlayingRoulette)
		{
			const PlayLayer* playLayer = GameManager::sharedState()->getPlayLayer();

			CCNode* normalPercentageNode = nullptr;
			float goalOffset = 24.f;

			if (playLayer->m_level->m_normalPercent < 10)
				goalOffset = 28.f;
			else if (playLayer->m_level->m_normalPercent <= 100)
				goalOffset = 40.f;

			CCObject* pauseLayerObject;
			CCARRAY_FOREACH(self->getChildren(), pauseLayerObject)
			{
				auto pauseLayerNode = reinterpret_cast<CCNode*>(pauseLayerObject);
				if (
					auto levelInfoLayerLabel = dynamic_cast<CCLabelBMFont*>(pauseLayerNode);
					levelInfoLayerLabel &&
					strcmp(levelInfoLayerLabel->getString(), CCString::createWithFormat("%d%%", playLayer->m_level->m_normalPercent)->getCString()) == 0
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
};
