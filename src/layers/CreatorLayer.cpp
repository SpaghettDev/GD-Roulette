#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RLRouletteLayer.hpp"

#include <Geode/Geode.hpp>
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

		auto menu = CCMenu::create();
		menu->setPosition({ 24.f, winSize.height / 2 });
		menu->setID("center-left-menu"_spr);
		this->addChild(menu);

		// Loader::get()->isModLoaded("absolllute.megahack")

		// this still sucks :(
		auto rouletteButton = CCMenuItemSpriteExtra::create(
			CCSprite::create("RL_blankBtn_001.png"_spr),
			this,
			menu_selector(RouletteButton::onRouletteButton)
		);
		rouletteButton->setID("roulette-button"_spr);
		auto rouletteSprite = CCSprite::create("RL_btn_001.png"_spr);
		rouletteSprite->setScale(.725f);
		rouletteSprite->setPosition({ 22.5f, 23.75f });
		rouletteButton->addChild(rouletteSprite);

		menu->addChild(rouletteButton);

		return true;
	}
};
