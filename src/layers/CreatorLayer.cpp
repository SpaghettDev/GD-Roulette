#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RLRouletteLayer.hpp"

#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;

class $modify(RouletteButton, CreatorLayer)
{
	void onRouletteButton(CCObject*)
	{
		if (g_rouletteManager.rouletteLayer = RLRouletteLayer::create(); g_rouletteManager.rouletteLayer)
			g_rouletteManager.rouletteLayer->show();

		if (auto exMark = this->getChildByID("center-left-menu"_spr)->getChildByID("roulette-button"_spr)->getChildByID("exclamation-mark"))
			exMark->removeFromParent();
	}

	bool init()
	{
		if (!CreatorLayer::init()) return false;

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		auto centerLeftMenu = CCMenu::create();
		centerLeftMenu->setPosition({
			this->getChildByID("exit-menu")->getPositionX(),
			CCDirector::sharedDirector()->getWinSize().height / 2.f
		});
		centerLeftMenu->setID("center-left-menu"_spr);
		this->addChild(centerLeftMenu);

		auto spr = CircleButtonSprite::createWithSprite("RL_btn_001.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Small);
		spr->setScale(1.1f);

		auto rouletteButton = CCMenuItemSpriteExtra::create(
			spr,
			this,
			menu_selector(RouletteButton::onRouletteButton)
		);
		rouletteButton->setID("roulette-button"_spr);
		rouletteButton->setLayout(AnchorLayout::create());
		centerLeftMenu->addChild(rouletteButton);

		if (g_rouletteManager.gameState.levelID != 0)
			g_rouletteManager.addExclamationMark(rouletteButton);

		return true;
	}
};
