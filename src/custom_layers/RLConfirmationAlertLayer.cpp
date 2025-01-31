#include "RLConfirmationAlertLayer.hpp"

using namespace geode::prelude;

RLConfirmationAlertLayer* RLConfirmationAlertLayer::create(const RLConfirmationData& cli)
{
	auto ret = new RLConfirmationAlertLayer();

	if (ret && ret->init(cli))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLConfirmationAlertLayer::init(const RLConfirmationData& cli)
{
	m_cli = cli;

	if (!this->createBasics({
		.contentSize = { 250.f, 150.f },
		.onClose = menu_selector(RLConfirmationAlertLayer::onClose),
		.bgTextureName = "square01_001.png",
		.bgRect = { .0f, .0f, 94.f, 94.f }
	})) return false;

	{ // modify BaseCustomAlertLayer
		this->closeBtn->setVisible(false);
	}

	auto titleText = CCLabelBMFont::create(m_cli.title.data(), "goldFont.fnt");
	titleText->setPosition({ .0f, 48.f });
	titleText->setScale(.9f);
	m_buttonMenu->addChild(titleText);

	auto textText = TextArea::create(m_cli.text.data(), "chatFont.fnt", 1.f, 190.f, { .5f, .5f }, 20.f, false);
	textText->setPosition({ .0f, 4.f });
	textText->setID("text-area");
	m_buttonMenu->addChild(textText);


	auto btn1 = CCMenuItemSpriteExtra::create(
		ButtonSprite::create(m_cli.btn1Text.data(), 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f),
		this,
		menu_selector(RLConfirmationAlertLayer::onYesButton)
	);
	btn1->setPosition({ m_cli.onBtn2 == nullptr ? .0f : -40.f, -44.f });
	btn1->setScale(.9f);
	btn1->setID("button-1");
	m_buttonMenu->addChild(btn1);

	auto btn2 = CCMenuItemSpriteExtra::create(
		ButtonSprite::create(m_cli.btn2Text.data(), 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f),
		this,
		menu_selector(RLConfirmationAlertLayer::onNoButton)
	);
	btn2->setPosition({ 50.f, -44.f });
	btn2->setScale(.9f);
	btn2->setVisible(m_cli.onBtn2 != nullptr);
	btn2->setID("button-2");
	m_buttonMenu->addChild(btn2);

	return true;
}

void RLConfirmationAlertLayer::onClose(CCObject*)
{
	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void RLConfirmationAlertLayer::onYesButton(CCObject*)
{
	m_cli.onBtn1(this);
	onClose(nullptr);
}

void RLConfirmationAlertLayer::onNoButton(CCObject*)
{
	m_cli.onBtn2(this);
	onClose(nullptr);
}
