#include <list>

#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

// not a field because the field dtor crashes the game idk why
std::list<int> levelIDs{};

class $modify(LevelInfoLayer)
{
	bool init(GJGameLevel* level, bool p1)
	{
		if (!LevelInfoLayer::init(level, p1)) return false;

		if (g_rouletteManager.isPlayingRoulette && level->m_levelID.value() == g_rouletteManager.currentLevelID)
		{
			levelIDs.push_back(level->m_levelID);

			CCLabelBMFont* normalPercentageLabel = as<CCLabelBMFont*>(this->getChildByID("normal-mode-percentage"));
			float goalOffset = .0f;

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
		if (levelIDs.size() != 1)
			levelIDs.pop_back();

		if (g_rouletteManager.isPlayingRoulette && g_rouletteManager.rouletteLayer && levelIDs.back() == g_rouletteManager.currentLevelID)
		{
			as<CCLabelBMFont*>(
				g_rouletteManager.rouletteLayer->m_pPlayingMenu->getChildByTag(20)
			)->setString(fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str());
		}

		if (levelIDs.size() == 1)
			levelIDs.pop_back();

		LevelInfoLayer::onBack(sender);
	}
};
