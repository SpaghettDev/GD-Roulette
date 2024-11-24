#include "../roulette/manager/RouletteManager.hpp"

#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class $modify(PauseLayer)
{
	void customSetup()
	{
		PauseLayer::customSetup();

		const auto* playLayer = GameManager::sharedState()->getPlayLayer();

		if (g_rouletteManager.isPlaying && playLayer->m_level->m_levelID.value() == g_rouletteManager.gameState.levelID)
		{
			CCLabelBMFont* normalPercentageLabel = static_cast<CCLabelBMFont*>(this->getChildByID("normal-progress-label"));

			if (!normalPercentageLabel) return;

			float goalOffset = normalPercentageLabel->getContentWidth() / 2 + 7.f;

			auto goalPercentage = CCLabelBMFont::create(
				fmt::format("({}%)", g_rouletteManager.currentPercentageGoal).c_str(),
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
