#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class $modify(PauseLayer)
{
	// TODO: return type is actually void, push to bindings
	void customSetup()
	{
		PauseLayer::customSetup();

		if (g_rouletteManager.isPlayingRoulette)
		{
			const PlayLayer* playLayer = GameManager::sharedState()->getPlayLayer();

			CCNode* normalPercentageLabel = nullptr;
			float goalOffset = 24.f;

			if (playLayer->m_level->m_normalPercent < 10)
				goalOffset = 28.f;
			else if (playLayer->m_level->m_normalPercent <= 100)
				goalOffset = 40.f;

			// normal mode percentage is always first in the array
			CCObject* pauseLayerObject;
			CCARRAY_FOREACH(this->getChildren(), pauseLayerObject)
			{
				if (
					auto levelInfoLayerLabel = typeinfo_cast<CCLabelBMFont*>(pauseLayerObject);
					levelInfoLayerLabel &&
					fmt::format("{}%", playLayer->m_level->m_normalPercent.value()) == levelInfoLayerLabel->getString()
					) {
					normalPercentageLabel = levelInfoLayerLabel;
					break;
				}
			};

			if (normalPercentageLabel == nullptr) return;

			auto goalPercentage = CCLabelBMFont::create(
				fmt::format("({}%)", static_cast<int>(g_rouletteManager.levelPercentageGoal)).c_str(),
				"bigFont.fnt"
			);
			goalPercentage->setPosition({ normalPercentageLabel->getPositionX() + goalOffset, normalPercentageLabel->getPositionY() });
			goalPercentage->setScale(.5f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(4);
			this->addChild(goalPercentage);
		}
	}
};
