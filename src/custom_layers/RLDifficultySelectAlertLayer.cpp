#include "RLDifficultySelectAlertLayer.hpp"
#include "constants.hpp"

#include <Geode/loader/Mod.hpp>
#include <Geode/ui/Layout.hpp>
#include <Geode/utils/cocos.hpp>

#include <Geode/binding/ButtonSprite.hpp>

using namespace geode::prelude;

RLDifficultySelectAlertLayer* RLDifficultySelectAlertLayer::create(const RLDifficultySelectData& dsi)
{
	auto ret = new RLDifficultySelectAlertLayer();

	if (ret && ret->init(dsi))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLDifficultySelectAlertLayer::init(const RLDifficultySelectData& dsi)
{
	m_dsi = dsi;

	if (!this->createBasics({
		.contentSize = { 400.f, 190.f },
		.onClose = menu_selector(RLDifficultySelectAlertLayer::onClose),
		.bgTextureName = "RL_square01_001.png"_spr,
		.bgRect = { .0f, .0f, 94.f, 94.f }
	})) return false;

	{ // modify BaseCustomAlertLayer
		closeBtn->setVisible(false);
	}

	difficulty_buttons_menu = CCMenu::create();
	difficulty_buttons_menu->setID("difficulty-buttons-menu");
	difficulty_buttons_menu->setContentSize({ 380.f, 180.f });
	difficulty_buttons_menu->setLayout(
		AxisLayout::create()
			->setGap(15.f)
			->setGrowCrossAxis(true)
	);
	difficulty_buttons_menu->setPositionY(165.f);
	m_mainLayer->addChild(difficulty_buttons_menu, 11);


	for (const auto& difficulty : m_dsi.difficulties)
	{
		auto difficultySprite = CCSprite::createWithSpriteFrameName(
			rl::constants::sprites::DIFFICULTY_TO_SPRITE.at(difficulty).data()
		);
		difficultySprite->setScale(1.2f);
		auto difficultyButton = CCMenuItemSpriteExtra::create(
			difficultySprite,
			this,
			menu_selector(RLDifficultySelectAlertLayer::onDemonButton)
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
		menu_selector(RLDifficultySelectAlertLayer::onClose)
	);
	okButton->setPosition({ .0f, -65.f });
	m_buttonMenu->addChild(okButton);


	return true;
}

void RLDifficultySelectAlertLayer::onClose(CCObject*)
{
	if (m_dsi.onOk)
		m_dsi.onOk(this);

	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void RLDifficultySelectAlertLayer::onDemonButton(CCObject* sender)
{
	const int tag = sender->getTag();

	m_dsi.onSelectDifficulty(static_cast<GJDifficulty>(tag), m_dsi.selectedDifficulty);
	m_dsi.selectedDifficulty = static_cast<GJDifficulty>(tag);

	for (auto* node : CCArrayExt<CCMenuItemSpriteExtra*>(difficulty_buttons_menu->getChildren()))
		node->setColor({ 128, 128, 128 });

	static_cast<CCMenuItemSpriteExtra*>(sender)->setColor({ 255, 255, 255 });
}

void RLDifficultySelectAlertLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape)
		onClose(nullptr);
}

void RLDifficultySelectAlertLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(
		this, cocos2d::kCCMenuHandlerPriority, true
	);
}
