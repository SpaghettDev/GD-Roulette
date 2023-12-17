#include "ConfirmationLayer.hpp"

ConfirmationAlertLayer* ConfirmationAlertLayer::create()
{
	auto ret = new ConfirmationAlertLayer();

	if (ret && ret->init())
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool ConfirmationAlertLayer::init()
{
	std::swap(m_tmp_cli, m_cli);

	if (!this->createBasics({ 250.f, 150.f }, menu_selector(ConfirmationAlertLayer::onClose), 1.f, { 0, 0, 0, 150 })) return false;

	{ // modify CustomLayer
		closeBtn->setVisible(false);

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		auto bg = reinterpret_cast<cocos2d::extension::CCScale9Sprite*>(
			m_pLayer->getChildren()->objectAtIndex(0)
		);
		auto bgContentSize = bg->getContentSize();
		auto bgPosition = bg->getPosition();
		bg->removeFromParentAndCleanup(true);

		auto newBg = cocos2d::extension::CCScale9Sprite::create("square01_001.png", { .0f, .0f, 94.f, 94.f });
		newBg->setContentSize(bgContentSize);
		newBg->setPosition(bgPosition);
		m_pLayer->addChild(newBg);
	}

	auto titleText = CCLabelBMFont::create(
		m_cli.title.data(), "goldFont.fnt"
	);
	titleText->setPosition({ .0f, 48.f });
	titleText->setScale(.9f);
	titleText->setTag(1);
	m_pButtonMenu->addChild(titleText);

	auto textText = gd::TextArea::create("chatFont.fnt", false, m_cli.text.data(), 1.f, 190.f, 20.f, {.5f, .5f});
	textText->setPosition({ .0f, 4.f });
	textText->setTag(2);
	m_pButtonMenu->addChild(textText);


	auto yesBtn = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create(m_cli.yesText.data(), 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f),
		this,
		menu_selector(ConfirmationAlertLayer::onYesButton)
	);
	yesBtn->setPosition({ m_cli.onNo == nullptr ? .0f : -40.f, -44.f });
	yesBtn->setScale(.9f);
	yesBtn->setTag(3);
	m_pButtonMenu->addChild(yesBtn);

	auto noBtn = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create(m_cli.noText.data(), 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f),
		this,
		menu_selector(ConfirmationAlertLayer::onNoButton)
	);
	noBtn->setPosition({ 50.f, -44.f });
	noBtn->setScale(.9f);
	noBtn->setVisible(m_cli.onNo != nullptr);
	noBtn->setTag(4);
	m_pButtonMenu->addChild(noBtn);


	return true;
}

void ConfirmationAlertLayer::onClose(CCObject*)
{
	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void ConfirmationAlertLayer::onYesButton(CCObject*)
{
	m_cli.onYes(this);
	onClose(nullptr);
}

void ConfirmationAlertLayer::onNoButton(CCObject*)
{
	m_cli.onNo(this);
	onClose(nullptr);
}
