#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;


class RouletteButton final : public FLAlertLayer
{
public:
	void onRouletteButton(CCObject*)
	{
		RouletteLayer::create()->show();
	}
};

class $modify(CreatorLayer)
{
	bool init()
	{
		if (!CreatorLayer::init()) return false;

		auto layer = reinterpret_cast<CCMenu*>(this->getChildren()->objectAtIndex(1));
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

		menu->addChild(rouletteButton);

		return true;
	}
};
