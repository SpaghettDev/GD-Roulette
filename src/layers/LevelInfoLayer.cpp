#include "RouletteManager.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>

#include "utils.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer)
{
	struct Fields
	{
		char m_update_label_values_step = 0;
	};

	bool init(GJGameLevel* level, bool p1)
	{
		if (!LevelInfoLayer::init(level, p1)) return false;

		const auto& rlm = RouletteManager::get();

		if (rlm.isPlaying && this->m_level->m_levelID.value() == rlm.gameState.levelID)
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

	void levelDownloadFinished(GJGameLevel* p0)
	{
		auto& rlm = RouletteManager::get();
		if (rlm.isPlaying && this->m_level->m_levelID.value() == rlm.gameState.levelID && rl::utils::isAprilFools())
		{
			p0->m_levelName = this->m_level->m_levelName;
			p0->m_creatorName = this->m_level->m_creatorName;
			p0->m_ratings = this->m_level->m_ratings;
			p0->m_ratingsSum = this->m_level->m_ratingsSum;
			p0->m_demon = this->m_level->m_demon;
			p0->m_demonDifficulty = this->m_level->m_demonDifficulty;
			p0->m_autoLevel = this->m_level->m_autoLevel;
			p0->m_stars = this->m_level->m_stars;
			p0->m_featured = this->m_level->m_featured;
			p0->m_isEpic = this->m_level->m_isEpic;
		}

		LevelInfoLayer::levelDownloadFinished(p0);
	}

	void levelUpdateFinished(GJGameLevel* p0, UpdateResponse p1)
	{
		auto& rlm = RouletteManager::get();
		if (rlm.isPlaying && this->m_level->m_levelID.value() == rlm.gameState.levelID && rl::utils::isAprilFools())
		{
			p0->m_levelName = this->m_level->m_levelName;
			p0->m_creatorName = this->m_level->m_creatorName;
			p0->m_ratings = this->m_level->m_ratings;
			p0->m_ratingsSum = this->m_level->m_ratingsSum;
			p0->m_demon = this->m_level->m_demon;
			p0->m_demonDifficulty = this->m_level->m_demonDifficulty;
			p0->m_autoLevel = this->m_level->m_autoLevel;
			p0->m_stars = this->m_level->m_stars;
			p0->m_featured = this->m_level->m_featured;
			p0->m_isEpic = this->m_level->m_isEpic;
		}

		LevelInfoLayer::levelUpdateFinished(p0, p1);
	}
};
