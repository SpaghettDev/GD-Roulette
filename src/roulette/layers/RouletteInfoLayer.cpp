#include "RouletteInfoLayer.hpp"
#include "../../utils.hpp"
#define DECLAREROULETTEMANAGER
#include "../manager/RouletteManager.hpp"
#include "../../custom_layers/IntegerInputLayer.hpp"

#include <matjson/stl_serialize.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

RouletteInfoLayer* RouletteInfoLayer::create()
{
	auto ret = new RouletteInfoLayer();

	if (ret && ret->init())
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RouletteInfoLayer::init()
{
	if (!this->createBasics({ 365.f, 240.f }, menu_selector(RouletteInfoLayer::onClose))) return false;

	auto infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { .0f, .0f, 80.0f, 80.0f });
	infoBg->setContentSize({ 320.f, 190.f });
	infoBg->setAnchorPoint({ .5f, 1.f });
	infoBg->setColor({ 123, 60, 31 });
	infoBg->setPosition({ .0f, 85.f });
	m_buttonMenu->addChild(infoBg, -1);


	auto infoTitle = CCLabelBMFont::create("GD Level Roulette Info", "goldFont.fnt");
	infoTitle->setPosition({ .0f, 102.f });
	infoTitle->setScale(.725f);
	m_buttonMenu->addChild(infoTitle);

	auto infoText = TextArea::create(
		"Welcome to the <cl>GD Level Roulette settings</c>!\n"
		"Here you can modify some <cy>settings</c> to your liking.",
		"chatFont.fnt",
		.85f, 290.f, { .5f, .5f }, 20.f, false
	);
	infoText->setPosition({ 27.f, 61.f });
	m_buttonMenu->addChild(infoText);

	createToggler(0, "Normal List", { -120.f, 15.f });
	createToggler(1, "Demon List", { 20.f, 15.f });
	createToggler(2, "Challenge List", { -120.f, -25.f });


	auto skipsButtonText = CCLabelBMFont::create("Number of Skips", "bigFont.fnt");
	skipsButtonText->setPosition({ 85.f, 16.f });
	skipsButtonText->setScale(.525f);
	auto skipsButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_longBtn03_001.png"),
		this,
		menu_selector(RouletteInfoLayer::onNumSkipsButton)
	);
	skipsButton->setPosition({ .0f, -65.f });
	skipsButton->addChild(skipsButtonText);
	skipsButton->setTag(3);
	m_buttonMenu->addChild(skipsButton);


	auto versionText = CCLabelBMFont::create(
#ifdef DEV_CONSOLE
		("Version " + Mod::get()->getVersion().toString(true) + " dev").c_str(),
#else
		("Version " + Mod::get()->getVersion().toString(true)).c_str(),
#endif // DEV_CONSOLE
		"bigFont.fnt"
	);
	versionText->setPosition({ .0f, -94.f });
	versionText->setScale(.5f);
	m_buttonMenu->addChild(versionText);


	return true;
}

void RouletteInfoLayer::destroyLayerChildren()
{
	for (unsigned int i = 0; i < this->getChildrenCount(); i++)
	{
		auto node = static_cast<CCNode*>(this->getChildren()->objectAtIndex(0));
		node->removeFromParentAndCleanup(true);
	}

	init();
}

void RouletteInfoLayer::onClose(CCObject*)
{
	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void RouletteInfoLayer::onToggleButton(CCObject* sender)
{
	sender->retain();

	auto button = static_cast<CCMenuItemToggler*>(sender);
	auto parent = static_cast<CCMenu*>(button->getParent());
	auto ind = roulette::utils::getIndexOf(RouletteManager.selectedListArr->as_array(), true);

	RouletteManager.selectedListArr->as_array().at(ind) = false;
	RouletteManager.selectedListArr->as_array().at(button->getTag()) = true;

	destroyLayerChildren();
	sender->release();
}

void RouletteInfoLayer::onNumSkipsButton(CCObject*)
{
	m_integer_input_layer->setup({
		"Number Of Skips", "Skips",
		0, 3, 9999, static_cast<int>(Mod::get()->getSettingValue<int64_t>("max-skips")),
		[&](auto iil) {
			Mod::get()->setSettingValue<int64_t>("max-skips", iil->m_integer);
		}
	});

	m_integer_input_layer = IntegerInputLayer::create();
	if (m_integer_input_layer)
		m_integer_input_layer->show();
}


CCMenuItemToggler* RouletteInfoLayer::createToggler(int tag, const char* labelText, CCPoint point, bool visible)
{
	auto buttonSpriteOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
	auto buttonSpriteOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
	buttonSpriteOn->setScale(.8f);
	buttonSpriteOff->setScale(.8f);

	auto button = CCMenuItemToggler::create(
		buttonSpriteOff,
		buttonSpriteOn,
		this,
		menu_selector(RouletteInfoLayer::onToggleButton)
	);
	button->setPosition(point);
	// button->setSizeMult(1.2f);
	button->setTag(tag);
	button->setVisible(visible);
	button->toggle(RouletteManager.selectedListArr->as_array().at(tag).as<bool>());
	m_buttonMenu->addChild(button);

	auto label = roulette::utils::createTextLabel(labelText, { point.x + 20, point.y }, .5f, m_buttonMenu);
	label->setAnchorPoint({ .0f, .5f });
	label->limitLabelWidth(80.f, .5f, 0);

	return button;
}
