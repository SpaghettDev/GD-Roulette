#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RLRouletteLayer.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer)
{
	bool init(GJGameLevel* level, bool p1)
	{
		if (!LevelInfoLayer::init(level, p1)) return false;

		if (g_rouletteManager.isPlaying && level->m_levelID.value() == g_rouletteManager.gameState.levelID)
		{
			CCLabelBMFont* normalPercentageLabel = static_cast<CCLabelBMFont*>(this->getChildByID("normal-mode-percentage"));

			if (!normalPercentageLabel) return true;

			float goalOffset = normalPercentageLabel->getContentWidth() / 2;

			auto goalPercentage = CCLabelBMFont::create(
				fmt::format("({}%)", g_rouletteManager.currentPercentageGoal).c_str(),
				"bigFont.fnt"
			);
			goalPercentage->setPosition({ normalPercentageLabel->getPositionX() + goalOffset, normalPercentageLabel->getPositionY() });
			goalPercentage->setScale(.4f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(3);
			goalPercentage->setID("goal-progress-label"_spr);
			this->addChild(goalPercentage);
		}

		return true;
	}

	void onBack(CCObject* sender)
	{
		if (
			g_rouletteManager.isPlaying &&
			g_rouletteManager.rouletteLayer &&
			this->m_level->m_levelID.value() == g_rouletteManager.gameState.levelID
		)
			static_cast<CCLabelBMFont*>(
				g_rouletteManager.rouletteLayer->playing_menu->getChildByID("percentage-text")
			)->setString(fmt::format("{}%", g_rouletteManager.currentPercentageGoal).c_str());

		LevelInfoLayer::onBack(sender);
	}
};
