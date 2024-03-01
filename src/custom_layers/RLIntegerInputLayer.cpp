#include "RLIntegerInputLayer.hpp"

RLIntegerInputLayer* RLIntegerInputLayer::create(const IntegerInputInfo& iili)
{
	auto ret = new RLIntegerInputLayer();

	if (ret && ret->init(iili))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLIntegerInputLayer::init(const IntegerInputInfo& iili)
{
	m_iili = iili;
	handleTouchPriority(this);
	if (!this->createBasics({ 175.f, 135.f }, menu_selector(RLIntegerInputLayer::onClose))) return false;
	this->closeBtn->setVisible(false);


	auto infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { .0f, .0f, 80.0f, 80.0f });
	infoBg->setContentSize({ 153.f, 95.f });
	infoBg->setAnchorPoint({ .5f, 1.f });
	infoBg->setColor({ 123, 60, 31 });
	infoBg->setPosition({ .0f, 37.f });
	m_buttonMenu->addChild(infoBg, -1);


	auto infoTitle = CCLabelBMFont::create(m_iili.title.data(), "goldFont.fnt");
	infoTitle->setPosition({ .0f, 52.f });
	infoTitle->limitLabelWidth(120.f, .8f, .5f);
	m_buttonMenu->addChild(infoTitle);


	auto integerBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
	integerBg->setPosition({ m_iili.show_arrows ? -30.f : -40.f, -10.f });
	integerBg->setContentSize({ m_iili.show_arrows ? 60.f : 80.f, 30.f });
	integerBg->setAnchorPoint({ .0f, .0f });
	integerBg->setColor({ 0, 0, 0 });
	integerBg->setOpacity(125);
	m_buttonMenu->addChild(integerBg, -1);

	auto leftButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"),
		this,
		menu_selector(RLIntegerInputLayer::onLeftButton)
	);
	leftButton->setPosition({ -42.f, 5.f });
	leftButton->setVisible(m_iili.show_arrows);
	leftButton->setID("left-button");
	m_buttonMenu->addChild(leftButton);

	auto rightButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"),
		this,
		menu_selector(RLIntegerInputLayer::onRightButton)
	);
	rightButton->setPosition({ 42.f, 5.f });
	rightButton->setVisible(m_iili.show_arrows);
	rightButton->setID("right-button");
	m_buttonMenu->addChild(rightButton);

	input_node = CCTextInputNode::create(m_iili.show_arrows ? 50.f : 70.f, 30.f, m_iili.input_text.data(), "bigFont.fnt");
	input_node->setLabelPlaceholderColor({ 0x75, 0xAA, 0xF0 });
	if (m_iili.starting_value != std::nullopt)
		input_node->setString(fmt::format("{}", m_iili.starting_value.value()).c_str());
	input_node->setAllowedChars("0123456789");
	input_node->setMaxLabelScale(.5f);
	input_node->setMaxLabelLength(5);
	input_node->setPosition({ .0f, 5.f });
	input_node->setID("input-node");
	m_buttonMenu->addChild(input_node);


	auto okButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("confirm", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, .85f),
		this,
		menu_selector(RLIntegerInputLayer::onClose)
	);
	okButton->setPosition({ .0f, -37.f });
	okButton->setScale(.925f);
	okButton->setID("confirm-button");
	m_buttonMenu->addChild(okButton);


	return true;
}

void RLIntegerInputLayer::onClose(CCObject*)
{
	try
	{
		m_integer = std::stoi(input_node->getString());
	}
	catch (...)
	{
		m_integer = m_iili.starting_value.value_or(0);
	}
	
	m_iili.onFinish(this);

	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void RLIntegerInputLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::KEY_Enter)
		onClose(nullptr);
}

void RLIntegerInputLayer::onLeftButton(CCObject*)
{
	try
	{
		m_integer = std::stoi(input_node->getString()) - 1;
	}
	catch (...)
	{
		m_integer = m_iili.fallback_value;
	}

	// manually sanitizing input because of mod menus
	if (m_integer >= m_iili.min_value)
		input_node->setString(fmt::format("{}", m_integer).c_str());
	else
		input_node->setString(fmt::format("{}", m_iili.fallback_value).c_str());
}

void RLIntegerInputLayer::onRightButton(CCObject*)
{
	try
	{
		m_integer = std::stoi(input_node->getString()) + 1;
	}
	catch (...)
	{
		m_integer = m_iili.fallback_value;
	}

	if (m_integer <= m_iili.max_value)
		input_node->setString(fmt::format("{}", m_integer).c_str());
	else
		input_node->setString(fmt::format("{}", m_iili.fallback_value).c_str());
}
