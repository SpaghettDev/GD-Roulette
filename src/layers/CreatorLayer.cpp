#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;

class $modify(RouletteButton, CreatorLayer)
{
	void onRouletteButton(CCObject*)
	{
		RouletteLayer::create()->show();
	}

	bool init()
	{
		if (!CreatorLayer::init()) return false;

		auto layer = static_cast<CCMenu*>(this->getChildren()->objectAtIndex(1));
		auto menu = CCMenu::create();
		menu->setID("creator-layer-menu"_spr);
		menu->setPosition({ .0f, .0f });
		this->addChild(menu);

		// Loader::get()->isModLoaded("absolllute.megahack")

		auto rouletteButton = CCMenuItemSpriteExtra::create(
			CCSprite::create("RL_blankBtn_001.png"_spr),
			layer,
			menu_selector(RouletteButton::onRouletteButton)
		);
		auto rouletteSprite = CCSprite::create("RL_btn_001.png"_spr);
		rouletteSprite->setScale(.725f);
		rouletteSprite->setPosition({ 22.25f, 23.75f });
		rouletteButton->addChild(rouletteSprite);
		rouletteButton->setPosition({ 25.f, 160.f });
		rouletteButton->setScale(.9f);

		menu->addChild(rouletteButton);

		return true;
	}
};
