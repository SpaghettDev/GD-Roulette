#include "RouletteInfoLayer.hpp"
#include "../../utils.hpp"
#define DECLAREROULETTEMANAGER
#include "../manager/RouletteManager.hpp"
#include "../../layers/IntegerInputLayer.hpp"
#include "../../json_manager/JsonManager.hpp"

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
	m_pButtonMenu->addChild(infoBg, -1);


	auto infoTitle = CCLabelBMFont::create("GD Level Roulette Info", "goldFont.fnt");
	infoTitle->setPosition({ .0f, 102.f });
	infoTitle->setScale(.725f);
	m_pButtonMenu->addChild(infoTitle);


	auto infoText1 = CCLabelBMFont::create("Welcome to GD Level Roulette!", "chatFont.fnt");
	auto infoText2 = CCLabelBMFont::create("Here you can modify some settings to your liking.", "chatFont.fnt");
	infoText1->setPosition({ .0f, 72.f });
	infoText2->setPosition({ .0f, 56.f });
	infoText1->setScale(.9f);
	infoText2->setScale(.85f);
	m_pButtonMenu->addChild(infoText1);
	m_pButtonMenu->addChild(infoText2);


	createToggler(0, "Normal List", { -120.f, 20.f });
	createToggler(1, "Demon List", { 20.f, 20.f });
	createToggler(2, "Challenge List", { -120.f, -20.f });


	auto skipsButtonText = CCLabelBMFont::create("Number of Skips", "bigFont.fnt");
	skipsButtonText->setPosition({ 85.f, 16.f });
	skipsButtonText->setScale(.525f);
	auto skipsButton = gd::CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_longBtn03_001.png"),
		this,
		menu_selector(RouletteInfoLayer::onSkipsButton)
	);
	skipsButton->setPosition({ .0f, -65.f });
	skipsButton->addChild(skipsButtonText);
	skipsButton->setTag(3);
	m_pButtonMenu->addChild(skipsButton);


	auto versionText = CCLabelBMFont::create(
#ifdef DEV_CONSOLE
		(std::string("Version ") + common::version.data() + " dev").c_str(),
#else
		(std::string("Version ") + common::version.data()).c_str(),
#endif // DEV_CONSOLE
		"bigFont.fnt"
	);
	versionText->setPosition({ .0f, -94.f });
	versionText->setScale(.5f);
	m_pButtonMenu->addChild(versionText);


	return true;
}

void RouletteInfoLayer::destroyLayerChildren()
{
	for (unsigned int i = 0; i < this->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<CCNode*>(this->getChildren()->objectAtIndex(0));
		node->removeFromParentAndCleanup(true);
	}

	init();
}

void RouletteInfoLayer::onClose(CCObject* sender)
{
	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void RouletteInfoLayer::onToggleButton(CCObject* sender)
{
	sender->retain();

	auto button = reinterpret_cast<gd::CCMenuItemToggler*>(sender);
	auto parent = reinterpret_cast<CCMenu*>(button->getParent());
	auto ind = utils::getIndexOf(RouletteManager.selectedListArr, true);

	RouletteManager.selectedListArr.at(ind) = false;
	RouletteManager.selectedListArr.at(button->getTag()) = true;

	destroyLayerChildren();
	sender->release();
}

void RouletteInfoLayer::onSkipsButton(CCObject* sender)
{
	IntegerInputLayer::create()->show();
}
