#include "RLDifficultySelectLayer.hpp"

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
	m_mainLayer->addChild(difficulty_buttons_menu, 11);


	for (int i = 0; i < m_dsi.difficulties.size(); i++)
	{
		const auto difficulty = m_dsi.difficulties.at(i);

		auto difficultySprite = CCSprite::createWithSpriteFrameName(
			fmt::format("difficulty_{0:0{1}}_btn2_001.png", static_cast<int>(difficulty), 2).c_str()
		);
		difficultySprite->setScale(1.2f);
		auto difficultyButton = CCMenuItemSpriteExtra::create(
			difficultySprite,
			this,
			menu_selector(RLDifficultySelectLayer::onDemonButton)
		);
		if (m_dsi.selectedDifficulty != difficulty)
			difficultyButton->setColor({ 125, 125, 125 });
		difficultyButton->setPosition({ -125.f + i * 60.f, 7.f });
		difficultyButton->setTag(static_cast<int>(difficulty));
		difficulty_buttons_menu->addChild(difficultyButton);
	}


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
