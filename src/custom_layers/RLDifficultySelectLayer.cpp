#include "RLDifficultySelectLayer.hpp"
#include "../utils.hpp"

RLDifficultySelectLayer* RLDifficultySelectLayer::create(const RLDifficultySelectInfo& dsi)
{
	auto ret = new RLDifficultySelectLayer();

	if (ret && ret->init(dsi))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLDifficultySelectLayer::init(const RLDifficultySelectInfo& dsi)
{
	m_dsi = dsi;
	if (!this->createBasics({ 380.f, 180.f }, menu_selector(RLDifficultySelectLayer::onClose), 1.f, { 0, 0, 0, 150 })) return false;

	{ // modify BaseCustomAlertLayer
		closeBtn->setVisible(false);
	}

	difficulty_buttons_menu = CCMenu::create();
	difficulty_buttons_menu->setID("difficulty-buttons");
	difficulty_buttons_menu->setContentSize({ 380.f, 180.f });
	difficulty_buttons_menu->setLayout(
		AxisLayout::create()
			->setGap(15.f)
			->setGrowCrossAxis(true)
	);
	m_mainLayer->addChild(difficulty_buttons_menu, 11);


	for (const auto& difficulty : m_dsi.difficulties)
	{
		auto difficultySprite = CCSprite::createWithSpriteFrameName(
			rl::constants::difficulty_to_sprite.at(difficulty).data()
		);
		difficultySprite->setScale(1.2f);
		auto difficultyButton = CCMenuItemSpriteExtra::create(
			difficultySprite,
			this,
			menu_selector(RLDifficultySelectLayer::onDemonButton)
		);
		if (m_dsi.selectedDifficulty != difficulty)
			difficultyButton->setColor({ 125, 125, 125 });
		difficultyButton->setLayoutOptions(AxisLayoutOptions::create());
		difficultyButton->setTag(static_cast<int>(difficulty));
		difficulty_buttons_menu->addChild(difficultyButton);
	}


	difficulty_buttons_menu->updateLayout();


	auto title = CCLabelBMFont::create(m_dsi.title.data(), "bigFont.fnt");
	title->setPosition({ .0f, 70.f });
	m_buttonMenu->addChild(title);

	auto okButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f),
		this,
		menu_selector(RLDifficultySelectLayer::onClose)
	);
	okButton->setPosition({ .0f, -65.f });
	m_buttonMenu->addChild(okButton);


	return true;
}

void RLDifficultySelectLayer::onClose(CCObject*)
{
	if (m_dsi.onOk)
		m_dsi.onOk(this);

	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void RLDifficultySelectLayer::onDemonButton(CCObject* sender)
{
	const int tag = sender->getTag();

	m_dsi.onSelectDifficulty(static_cast<GJDifficulty>(tag), m_dsi.selectedDifficulty);
	m_dsi.selectedDifficulty = static_cast<GJDifficulty>(tag);

	for (auto* node : CCArrayExt<CCMenuItemSpriteExtra*>(difficulty_buttons_menu->getChildren()))
		node->setColor({ 125, 125, 125 });

	static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({ 255, 255, 255 });
}

void RLDifficultySelectLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape)
		onClose(nullptr);
}

void RLDifficultySelectLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority, true);
}
