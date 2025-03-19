#include "RouletteManager.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer)
{
	bool init(GJGameLevel* level, bool p1)
	{
		if (!LevelInfoLayer::init(level, p1)) return false;

		const auto& rlm = RouletteManager::get();

		if (rlm.isPlaying && level->m_levelID.value() == rlm.gameState.levelID)
		{
			CCLabelBMFont* normalPercentageLabel = static_cast<CCLabelBMFont*>(this->getChildByID("normal-mode-percentage"));

			if (!normalPercentageLabel) return true;

			auto goalPercentage = CCLabelBMFont::create(
				fmt::format("({}%)", rlm.currentPercentageGoal).c_str(),
				"bigFont.fnt"
			);
			goalPercentage->setScale(.4f);

			float goalOffset = (
				(normalPercentageLabel->getContentWidth() * normalPercentageLabel->getScaleX()) / 2.f
			) + (
				(goalPercentage->getContentWidth() * goalPercentage->getScaleX()) / 2.f
			) + 5.f;

			goalPercentage->setPosition({
				normalPercentageLabel->getPositionX() + goalOffset,
				normalPercentageLabel->getPositionY()
			});
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(3);
			goalPercentage->setID("goal-progress-label"_spr);
			this->addChild(goalPercentage);
		}

		return true;
	}
};
