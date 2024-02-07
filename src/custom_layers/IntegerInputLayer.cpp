#include "IntegerInputLayer.hpp"

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
	std::swap(m_tmp_iili, m_iili);

	if (!this->createBasics({ 175.f, 135.f }, menu_selector(IntegerInputLayer::onClose))) return false;

	this->closeBtn->setVisible(false);
	this->setTouchPriority(CCDirector::sharedDirector()->getTouchDispatcher()->getTargetPrio());


	auto infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { .0f, .0f, 80.0f, 80.0f });
	infoBg->setContentSize({ 153.f, 95.f });
	infoBg->setAnchorPoint({ .5f, 1.f });
	infoBg->setColor({ 123, 60, 31 });
	infoBg->setPosition({ .0f, 37.f });
	m_buttonMenu->addChild(infoBg, -1);


	auto infoTitle = CCLabelBMFont::create(m_iili.title.data(), "goldFont.fnt");
	infoTitle->setPosition({ .0f, 52.f });
	infoTitle->setScale(.575f);
	m_buttonMenu->addChild(infoTitle);


	auto integerBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
	integerBg->setPosition({ -30.f, -5.f });
	integerBg->setContentSize({ 60.f, 30.f });
	integerBg->setAnchorPoint({ .0f, .0f });
	integerBg->setColor({ 0, 0, 0 });
	integerBg->setOpacity(125);
	m_buttonMenu->addChild(integerBg, -1);

	auto leftButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"),
		this,
		menu_selector(IntegerInputLayer::onLeftButton)
	);
	leftButton->setPosition({ -42.f, 10.f });
	m_buttonMenu->addChild(leftButton);

	auto rightButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"),
		this,
		menu_selector(IntegerInputLayer::onRightButton)
	);
	rightButton->setPosition({ 42.f, 10.f });
	m_buttonMenu->addChild(rightButton);

	auto integerInput = CCTextInputNode::create(100, 30, m_iili.input_text.data(), "bigFont.fnt");
	integerInput->setLabelPlaceholderColor({ 0x75, 0xAA, 0xF0 });
	integerInput->setString(fmt::format("{}", m_iili.starting_value).c_str());
	integerInput->setAllowedChars("0123456789");
	integerInput->setMaxLabelScale(.5f);
	integerInput->setMaxLabelLength(5);
	integerInput->setPosition({ .0f, 10.f });
	integerInput->setTag(1);
	m_buttonMenu->addChild(integerInput);


	auto okBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("confirm", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, .85f),
		this,
		menu_selector(IntegerInputLayer::onClose)
	);
	okBtn->setPosition({ .0f, -37.f });
	okBtn->setScale(.925f);
	m_buttonMenu->addChild(okBtn);


	return true;
}

void IntegerInputLayer::onClose(CCObject*)
{
	try
	{
		m_integer = std::stoi(
			as<CCTextInputNode*>(m_buttonMenu->getChildByTag(1))->getString()
		);

	}
	catch (...)
	{
		m_integer = m_iili.starting_value;
	}
	
	m_iili.onFinish(this);

	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void IntegerInputLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::KEY_Enter)
		onClose(nullptr);
}

void IntegerInputLayer::onLeftButton(CCObject*)
{
	const auto inputNode = as<CCTextInputNode*>(m_buttonMenu->getChildByTag(1));

	try
	{
		m_integer = std::stoi(inputNode->getString()) - 1;
	}
	catch (...)
	{
		m_integer = m_iili.fallback_value;
	}

	// manually sanitizing input because of mod menus
	if (m_integer > m_iili.min_value)
		inputNode->setString(fmt::format("{}", m_integer).c_str());
	else
		inputNode->setString(fmt::format("{}", m_iili.fallback_value).c_str());
}

void IntegerInputLayer::onRightButton(CCObject*)
{
	const auto inputNode = as<CCTextInputNode*>(m_buttonMenu->getChildByTag(1));

	try
	{
		m_integer = std::stoi(inputNode->getString()) + 1;
	}
	catch (...)
	{
		m_integer = m_iili.fallback_value;
	}

	if (m_integer < m_iili.max_value)
		inputNode->setString(fmt::format("{}", m_integer).c_str());
	else
		inputNode->setString(fmt::format("{}", m_iili.fallback_value).c_str());
}
