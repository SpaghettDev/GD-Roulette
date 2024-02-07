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

		const PlayLayer* playLayer = GameManager::sharedState()->getPlayLayer();

		if (g_rouletteManager.isPlayingRoulette && playLayer->m_level->m_levelID.value() == g_rouletteManager.currentLevelID)
		{
			CCLabelBMFont* normalPercentageLabel = as<CCLabelBMFont*>(this->getChildByID("normal-progress-label"));
			float goalOffset = 24.f;

			if (playLayer->m_level->m_normalPercent < 10)
				goalOffset = 28.f;
			else if (playLayer->m_level->m_normalPercent <= 100)
				goalOffset = 40.f;
			auto goalPercentage = CCLabelBMFont::create(
				fmt::format("({}%)", g_rouletteManager.levelPercentageGoal).c_str(),
				"bigFont.fnt"
			);
			goalPercentage->setPosition({ normalPercentageLabel->getPositionX() + goalOffset, normalPercentageLabel->getPositionY() });
			goalPercentage->setScale(.5f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(4);
			goalPercentage->setID("goal-progress-label"_spr);
			this->addChild(goalPercentage);
		}
	}
};
