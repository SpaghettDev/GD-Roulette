#include "RLRepeatingBackground.hpp"

#ifndef ROULETTE_DEBUG_BUILD
#include "utils.hpp"
#endif

using namespace geode::prelude;

RLRepeatingBackground* RLRepeatingBackground::create(const char* texture)
{
	auto ret = new RLRepeatingBackground();

	if (ret && ret->init(texture))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLRepeatingBackground::init(const char* texture)
{
	if (!CCLayer::init()) return false;

#ifdef ROULETTE_DEBUG_BUILD
	auto peggyChance = true;
#else
	auto peggyChance = rl::utils::randomNumber(0, 10000) == 69;
#endif

	bg1 = CCSprite::create(texture);
	bg1->setAnchorPoint({ .0f, .0f });
	bg1->setPosition({ .0f, .0f });
	bg1->setID("background-1");
	this->addChild(bg1, -1);

	bg2 = CCSprite::create(texture);
	bg2->setAnchorPoint({ .0f, .0f });
	bg2->setPosition({ bg2->getContentWidth() - 1.f, .0f }); // -1 to avoid a gap
	bg2->setID("background-2");
	this->addChild(bg2, -1);

	bg3 = CCSprite::create(
		peggyChance
			? "RL_game_bg_100_001.png"_spr
			: texture
	);
	if (peggyChance) bg3->setScaleY(.625f);
	bg3->setAnchorPoint({ .0f, .0f });
	bg3->setPosition({ (2.f * bg3->getContentWidth()) - 1.f, .0f }); // -1 to avoid a gap
	bg3->setID("background-3");
	this->addChild(bg3, -1);

#ifdef ROULETTE_DEBUG_BUILD
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	auto peggyBg = CCSprite::create("RL_game_bg_100_001.png"_spr);
	peggyBg->setScale(.325f);
	peggyBg->setColor({ 50, 50, 50 });
	peggyBg->setPosition({ .0f, winSize.height / 2.f + peggyBg->getScaledContentHeight() / 2.f });
	bg2->addChild(peggyBg);
#endif

	start();

	return true;
}

void RLRepeatingBackground::setColor(const cocos2d::ccColor3B& color)
{
	bg1->setColor(color);
	bg2->setColor(color);
	bg3->setColor(color);
}

void RLRepeatingBackground::update(float dt)
{
	float newBG1PosX = bg1->getPositionX() - SCROLL_SPEED * dt;
	float newBG2PosX = bg2->getPositionX() - SCROLL_SPEED * dt;
	float newBG3PosX = bg3->getPositionX() - SCROLL_SPEED * dt;

	bg1->setPositionX(newBG1PosX);
	bg2->setPositionX(newBG2PosX);
	bg3->setPositionX(newBG3PosX);

	if (newBG1PosX <= -bg1->getContentWidth())
		bg1->setPositionX(newBG3PosX + bg3->getContentWidth() - 1.f);

	if (newBG2PosX <= -bg2->getContentWidth())
		bg2->setPositionX(newBG1PosX + bg1->getContentWidth() - 1.f);

	if (newBG3PosX <= -bg3->getContentWidth())
		bg3->setPositionX(newBG2PosX + bg2->getContentWidth() - 1.f);
}

void RLRepeatingBackground::start()
{
	this->scheduleUpdate();
}

void RLRepeatingBackground::stop()
{
	this->unscheduleUpdate();
}
