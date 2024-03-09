#include <list>

#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RLRouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer)
{
	bool init(GJGameLevel* level, bool p1)
	{
		if (!LevelInfoLayer::init(level, p1)) return false;

		if (g_rouletteManager.isPlaying && level->m_levelID.value() == g_rouletteManager.currentLevelID)
		{
			CCLabelBMFont* normalPercentageLabel = static_cast<CCLabelBMFont*>(this->getChildByID("normal-mode-percentage"));
			float goalOffset = .0f;

			if (!normalPercentageLabel) return true;

			// wtf v2
			if (this->m_level->m_normalPercent < 10)
				goalOffset = 24.f;
			else if (this->m_level->m_normalPercent < 100)
				goalOffset = 31.f;
			else
				goalOffset = 39.f;

			auto goalPercentage = CCLabelBMFont::create(
				fmt::format("({}%)", g_rouletteManager.levelPercentageGoal).c_str(),
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
			this->m_level->m_levelID.value() == g_rouletteManager.currentLevelID
		)
			static_cast<CCLabelBMFont*>(
				g_rouletteManager.rouletteLayer->playing_menu->getChildByID("percentage-text")
			)->setString(fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str());

		LevelInfoLayer::onBack(sender);
	}
};
