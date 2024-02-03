#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"
#include "../custom_layers/CustomDirector.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer)
{
	bool init(GJGameLevel* level, bool p1)
	{
		if (!LevelInfoLayer::init(level, p1)) return false;

		if (RouletteManager.isPlayingRoulette)
		{
			CCLabelBMFont* normalPercentageLabel = static_cast<CCLabelBMFont*>(this->getChildByID("normal-mode-percentage"));
			float goalOffset = .0f;

			if (this->m_level->m_normalPercent < 10)
				goalOffset = 24.f;
			else if (this->m_level->m_normalPercent <= 100)
				goalOffset = 31.f;

			if (normalPercentageLabel == nullptr) return true;

			auto goalPercentage = CCLabelBMFont::create(
				fmt::format("({}%)", static_cast<int>(RouletteManager.levelPercentageGoal)).c_str(),
				"bigFont.fnt"
			);
			goalPercentage->setPosition({ normalPercentageLabel->getPositionX() + goalOffset, normalPercentageLabel->getPositionY() });
			goalPercentage->setScale(.4f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(3);
			this->addChild(goalPercentage);
		}

		return true;
	}

	void onBack(CCObject* sender)
	{
		CustomDirector* director = static_cast<CustomDirector*>(CCDirector::sharedDirector());

		if (RouletteManager.isPlayingRoulette && this->m_level->m_levelID == RouletteManager.lastLevelID)
		{
			if (
				CCScene* prevScene = director->getPreviousScene();
				prevScene->getChildrenCount() == 2
				) {
				if (auto rouletteLayer = typeinfo_cast<RouletteLayer*>(prevScene->getChildren()->objectAtIndex(1)))
				{
					static_cast<CCLabelBMFont*>(
						rouletteLayer->m_pPlayingMenu->getChildByTag(20)
					)->setString(fmt::format("{}%", RouletteManager.levelPercentageGoal).c_str());
				}
			}
		}

		LevelInfoLayer::onBack(sender);
	}
};
