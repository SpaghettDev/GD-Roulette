#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RLRouletteLayer.hpp"

#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;

class $modify(RouletteButton, CreatorLayer)
{
	void onRouletteButton(CCObject*)
	{
		g_rouletteManager.rouletteLayer = RLRouletteLayer::create();
		if (g_rouletteManager.rouletteLayer)
			g_rouletteManager.rouletteLayer->show();
	}

	bool init()
	{
		if (!CreatorLayer::init()) return false;

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		if(Mod::get()->getSettingValue<bool>("use-big-button") && Loader::get()->isModLoaded("alphalaneous.pages_api")) {
			if(auto menu = getChildByID("creator-buttons-menu")) {
				auto spr = CCSprite::create("RL_btn_big.png"_spr);
				spr->setScale(0.842);
				auto rouletteButton = CCMenuItemSpriteExtra::create(
					spr,
					this,
					menu_selector(RouletteButton::onRouletteButton)
				);
				rouletteButton->setID("roulette-button"_spr);
				menu->addChild(rouletteButton);
			}
		}
		else {
			if(auto menu = getChildByID("exit-menu")) {
				auto spr = CircleButtonSprite::createWithSprite("RL_btn_001.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Small);

				auto rouletteButton = CCMenuItemSpriteExtra::create(
					spr,
					this,
					menu_selector(RouletteButton::onRouletteButton)
				);
				rouletteButton->setID("roulette-button"_spr);
				menu->addChild(rouletteButton);
				typeinfo_cast<AxisLayout*>(menu->getLayout())->setAxisReverse(true);
				menu->updateLayout();
			}
		}

		return true;
	}
};
