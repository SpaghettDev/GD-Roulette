#include "RouletteManager.hpp"

#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(RLMenuLayer, MenuLayer)
{
	bool init()
	{
		if (!MenuLayer::init()) return false;

		if (Loader::get()->isModLoaded("ninxout.redash"))
		{
			if (Mod::get()->getSettingValue<bool>("use-big-button"))
				Mod::get()->setSettingValue("use-big-button", false);
		}

		if (Loader::get()->isModLoaded("ninxout.redash"))
		{
			auto rouletteButton = static_cast<CCMenuItemSpriteExtra*>(
				this->querySelector(
					fmt::format("right-side-menu > {}", "roulette-button"_spr)
				)
			);

			// where tf my button go??
			if (!rouletteButton)
				return true;

			if (RouletteManager::get().isPaused)
				RouletteManager::addExclamationMark(rouletteButton);
			else if (auto exMark = rouletteButton->getChildByID("exclamation-mark"_spr))
				exMark->removeFromParentAndCleanup(true);
		}

		return true;
	}
};
