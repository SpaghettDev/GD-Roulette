#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;

class $modify(RouletteButton, CreatorLayer)
{
	void onRouletteButton(CCObject*)
	{
		g_rouletteManager.rouletteLayer = RouletteLayer::create();
		if (g_rouletteManager.rouletteLayer)
			g_rouletteManager.rouletteLayer->show();
	}

	bool init()
	{
		if (!CreatorLayer::init()) return false;

		auto menu = CCMenu::create();
		menu->setID("creator-layer-menu"_spr);
		menu->setPosition({ .0f, .0f });
		this->addChild(menu);

		// Loader::get()->isModLoaded("absolllute.megahack")

		// this is, well uhhh, very awful
		auto rouletteButton = CCMenuItemSpriteExtra::create(
			CCSprite::create("RL_blankBtn_001.png"_spr),
			this,
			menu_selector(RouletteButton::onRouletteButton)
		);
		auto rouletteSprite = CCSprite::create("RL_btn_001.png"_spr);
		rouletteSprite->setScale(.725f);
		rouletteSprite->setPosition({ 22.25f, 23.75f });
		rouletteButton->addChild(rouletteSprite);
		rouletteButton->setPosition({ 25.f, 160.f });

		menu->addChild(rouletteButton);

		return true;
	}
};
