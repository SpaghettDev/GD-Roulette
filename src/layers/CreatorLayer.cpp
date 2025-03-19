#include "RouletteManager.hpp"
#include "../roulette/RLRouletteLayer.hpp"

#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

using namespace geode::prelude;

class $modify(RLCreatorLayer, CreatorLayer)
{
	void onRouletteButton(CCObject*)
	{
		auto& rlm = RouletteManager::get();

		if ((rlm.rouletteLayer = RLRouletteLayer::create()))
		{
			auto scene = CCScene::create();
			scene->addChild(rlm.rouletteLayer);

			CCDirector::sharedDirector()->pushScene(
				CCTransitionFade::create(.5f, scene)
			);
		}
	}

	bool init()
	{
		if (!CreatorLayer::init()) return false;

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		CCMenuItemSpriteExtra* rouletteButton;

		if (!Mod::get()->getSettingValue<bool>("use-big-button"))
		{
			auto centerLeftMenu = CCMenu::create();
			centerLeftMenu->setPosition({
				this->getChildByID("exit-menu")->getPositionX(),
				CCDirector::sharedDirector()->getWinSize().height / 2.f
			});
			centerLeftMenu->setID("center-left-menu"_spr);
			this->addChild(centerLeftMenu);

			auto rouletteButtonSprite = CircleButtonSprite::createWithSprite(
				"RL_btn_001.png"_spr,
				1.f,
				CircleBaseColor::Green,
				CircleBaseSize::Small
			);
			rouletteButtonSprite->setScale(1.1f);

			rouletteButton = CCMenuItemSpriteExtra::create(
				rouletteButtonSprite,
				this,
				menu_selector(RLCreatorLayer::onRouletteButton)
			);
			rouletteButton->setID("roulette-button"_spr);
			rouletteButton->setLayout(AnchorLayout::create());
			centerLeftMenu->addChild(rouletteButton);
		}
		else
		{
			auto rouletteButtonSprite = CCSprite::create("RL_btn_big.png"_spr);
			rouletteButtonSprite->setScale(.8f);
			rouletteButton = CCMenuItemSpriteExtra::create(
				rouletteButtonSprite,
				this,
				menu_selector(RLCreatorLayer::onRouletteButton)
			);
			rouletteButton->setID("roulette-button"_spr);
			rouletteButton->setLayout(AnchorLayout::create());
			this->getChildByID("creator-buttons-menu")->addChild(rouletteButton);
			this->getChildByID("creator-buttons-menu")->updateLayout();
		}

		if (RouletteManager::get().isPaused)
			RouletteManager::addExclamationMark(rouletteButton);

		return true;
	}
};
