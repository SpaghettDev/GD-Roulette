#include "IntegerInputLayer.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../json_manager/JsonManager.hpp"

IntegerInputLayer* IntegerInputLayer::create()
{
	auto ret = new IntegerInputLayer();

	if (ret && ret->init())
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool IntegerInputLayer::init()
{
	if (!this->createBasics({ 200.f, 100.f }, menu_selector(IntegerInputLayer::onClose))) return false;

	auto infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { .0f, .0f, 80.0f, 80.0f });
	infoBg->setContentSize({ 175.f, 65.f });
	infoBg->setAnchorPoint({ .5f, 1.f });
	infoBg->setColor({ 123, 60, 31 });
	infoBg->setPosition({ .0f, 27.f });
	m_pButtonMenu->addChild(infoBg, -1);


	auto infoTitle = CCLabelBMFont::create("Number Of Skips", "goldFont.fnt");
	infoTitle->setPosition({ .0f, 38.f });
	infoTitle->setScale(.575f);
	m_pButtonMenu->addChild(infoTitle);


	auto maxSkipsBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
	maxSkipsBg->setPosition({ -30.f, -20.f });
	maxSkipsBg->setContentSize({ 60.f, 30.f });
	maxSkipsBg->setAnchorPoint({ .0f, .0f });
	maxSkipsBg->setColor({ 0, 0, 0 });
	maxSkipsBg->setOpacity(125);
	m_pButtonMenu->addChild(maxSkipsBg, -1);

	auto leftButton = gd::CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"),
		this,
		menu_selector(IntegerInputLayer::onLeftButton)
	);
	leftButton->setPosition({ -42.f, -5.f });
	m_pButtonMenu->addChild(leftButton);

	auto rightButton = gd::CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"),
		this,
		menu_selector(IntegerInputLayer::onRightButton)
	);
	rightButton->setPosition({ 42.f, -5.f });
	m_pButtonMenu->addChild(rightButton);

	auto skipsInput = gd::CCTextInputNode::create("Skips", this, "bigFont.fnt", 100, 30);
	skipsInput->setLabelPlaceholderColor({ 0x75, 0xAA, 0xF0 });
	skipsInput->setString(CCString::createWithFormat("%d", RouletteManager.maxSkips)->getCString());
	skipsInput->setAllowedChars("0123456789");
	skipsInput->setMaxLabelScale(.5f);
	skipsInput->setMaxLabelLength(5);
	skipsInput->setPosition({ .0f, -5.f });
	skipsInput->setTag(1);
	m_pButtonMenu->addChild(skipsInput);


	return true;
}

void IntegerInputLayer::onClose(CCObject* sender)
{
	try
	{
		const int skips = std::stoi(
			reinterpret_cast<gd::CCTextInputNode*>(m_pButtonMenu->getChildByTag(1))->getString()
		);

		RouletteManager.maxSkips = skips < 0 ? 3 : skips;
	}
	catch (...)
	{
		RouletteManager.maxSkips = 3;
	}

	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void IntegerInputLayer::onLeftButton(CCObject* sender)
{
	const auto inputNode = reinterpret_cast<gd::CCTextInputNode*>(m_pButtonMenu->getChildByTag(1));
	int previousValue;

	try
	{
		previousValue = std::stoi(inputNode->getString());
	}
	catch (...)
	{
		previousValue = 0;
	}

	// manually sanitizing input because of mod menus
	if (previousValue > 0)
		inputNode->setString(std::to_string(previousValue - 1).c_str());
	else
		inputNode->setString("0");
}

void IntegerInputLayer::onRightButton(CCObject* sender)
{
	const auto inputNode = reinterpret_cast<gd::CCTextInputNode*>(m_pButtonMenu->getChildByTag(1));
	int previousValue;

	try
	{
		previousValue = std::stoi(inputNode->getString());
	}
	catch (...)
	{
		previousValue = 0;
	}

	if (previousValue < 99999)
		inputNode->setString(std::to_string(previousValue + 1).c_str());
	else
		inputNode->setString("99999");
}
