#include "RLRouletteInfoAlertLayer.hpp"

#include "../custom_layers/RLIntegerInputAlertLayer.hpp"
#include "../roulette/RLRouletteLayer.hpp"
#include "RouletteManager.hpp"
#include "DataManager.hpp"
#include "utils.hpp"

using namespace geode::prelude;

RLRouletteInfoAlertLayer* RLRouletteInfoAlertLayer::create()
{
	auto ret = new RLRouletteInfoAlertLayer();

	if (ret && ret->init())
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLRouletteInfoAlertLayer::init()
{
	if (!this->createBasics({
		.contentSize = { 365.f, 240.f },
		.onClose = menu_selector(RLRouletteInfoAlertLayer::onClose),
		.bgTextureName = "RL_square01_001.png"_spr,
		.bgRect = { .0f, .0f, 94.f, 94.f }
	})) return false;

	{
		auto winSize = CCDirector::sharedDirector()->getWinSize();

		auto infoBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { .0f, .0f, 80.0f, 80.0f });
		infoBg->setContentSize({ 320.f, 160.f });
		infoBg->setAnchorPoint({ .5f, .5f });
		infoBg->setColor({ 50, 50, 50 });
		infoBg->setPosition({ winSize.width / 2.f, winSize.height / 2.f });
		m_mainLayer->addChild(infoBg, -1);
	}


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

	createToggler(0, "normal-list", "Normal List", { -120.f, 15.f });
	createToggler(1, "demon-list", "Demon List", { 20.f, 15.f });
	createToggler(2, "challenge-list", "Challenge List", { -120.f, -25.f });
	createToggler(3, "gd-list", "GD List", { 20.f, -25.f });

	auto idSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
	idSprite->setScale(.5f);
	auto idButton = CCMenuItemSpriteExtra::create(
		idSprite,
		this,
		menu_selector(RLRouletteInfoAlertLayer::onIDButton)
	);
	idButton->setVisible(
		rl::utils::getIndexOf(
			DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().asVector(), true
		) == 3
	);
	{
		auto listBtnPos = m_buttonMenu->getChildByID("gd-list")->getPosition();

		idButton->setPosition({ listBtnPos.x + 100.f, listBtnPos.y });
	}
	idButton->setID("list-id-button");
	m_buttonMenu->addChild(idButton);


	auto skipsButtonText = CCLabelBMFont::create("Number of Skips", "bigFont.fnt");
	skipsButtonText->setPosition({ 85.f, 16.f });
	skipsButtonText->setScale(.525f);
	auto skipsButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_longBtn03_001.png"),
		this,
		menu_selector(RLRouletteInfoAlertLayer::onNumSkipsButton)
	);
	skipsButton->setPosition({ .0f, -60.f });
	skipsButton->addChild(skipsButtonText);
	skipsButton->setID("skips-button");
	m_buttonMenu->addChild(skipsButton);


	auto versionText = CCLabelBMFont::create(
#if defined(ROULETTE_DEBUG_BUILD) && defined(GEODE_PLATFORM_SHORT_IDENTIFIER)
		fmt::format("Version: {} ({} dev)", Mod::get()->getVersion().toVString(true), GEODE_PLATFORM_SHORT_IDENTIFIER).c_str(),
#elif defined(GEODE_PLATFORM_SHORT_IDENTIFIER)
		fmt::format("Version: {} ({})", Mod::get()->getVersion().toVString(true), GEODE_PLATFORM_SHORT_IDENTIFIER).c_str(),
#else
		fmt::format("Version: {} (how)", Mod::get()->getVersion().toVString(true)).c_str(),
#endif
		"bigFont.fnt"
	);
	versionText->setPosition({ .0f, -95.f });
	versionText->setScale(.5f);
	m_buttonMenu->addChild(versionText);


	return true;
}

void RLRouletteInfoAlertLayer::onClose(CCObject*)
{
	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void RLRouletteInfoAlertLayer::onToggleButton(CCObject* sender)
{
	auto button = static_cast<CCMenuItemToggler*>(sender);
	auto prevIdx = rl::utils::getIndexOf(
		DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().asVector(), true
	);

	DataManager::set<DMArrayKey::SELECTED_LIST_ARRAY>(prevIdx, false);
	DataManager::set<DMArrayKey::SELECTED_LIST_ARRAY>(button->getTag(), true);

	RouletteManager::get().rouletteLayer->onListChanged();

	m_buttonMenu->getChildByID("list-id-button")->setVisible(button->getTag() == 3);

	for (int i = 0; i < 4; i++)
		static_cast<CCMenuItemToggler*>(m_buttonMenu->getChildByTag(i))->toggle(false);
}

void RLRouletteInfoAlertLayer::onNumSkipsButton(CCObject*)
{
	m_integer_input_layer = RLIntegerInputAlertLayer::create({
		"Number Of Skips", "Skips", 0, 3, 9999,
		static_cast<int>(Mod::get()->getSettingValue<int64_t>("max-skips")),
		4, true,
		[&](auto iil) {
			Mod::get()->setSettingValue<int64_t>("max-skips", iil->m_integer);
		}
	});
	if (m_integer_input_layer)
		m_integer_input_layer->show();
}

void RLRouletteInfoAlertLayer::onInfoIcon(CCObject* sender)
{
	auto button = static_cast<CCMenuItemSpriteExtra*>(sender);

	if (button->getID() == "normal-list-info")
	{
		FLAlertLayer::create(
			nullptr,
			"Normal List",
			"Levels ranging from the <cl>Easy</c> difficulty to the <cr>Extreme Demon</c> difficulty.\nOnly rated levels appear in this list.",
			"OK",
			nullptr
		)->show();
	}
	else if (button->getID() == "demon-list-info")
	{
		FLAlertLayer::create(
			nullptr,
			"Demon List",
			"The Pointercrate demon list.\n<cy>Only</c> includes levels from the regular demon list, <cr>and not</c> the extended list.\nSource: https://pointercrate.com/demonlist",
			"OK",
			nullptr
		)->show();
	}
	else if (button->getID() == "challenge-list-info")
	{
		FLAlertLayer::create(
			nullptr,
			"Challenge List",
			"The Challenge list.\nSame as the demon list, as in the levels from the extended list <cr>don't appear</c> in the roulette.\nSource: https://challengelist.gd/challenges",
			"OK",
			nullptr
		)->show();
	}
	else if (button->getID() == "gd-list-info")
	{
		FLAlertLayer::create(
			nullptr,
			"GD List",
			"A GD List.\nPretty self-explanatory.\n<cr>Platformer levels appear in this list!</c>\n<cy>No</c>, i will not fix :D",
			"OK",
			nullptr
		)->show();
	}
}

void RLRouletteInfoAlertLayer::onIDButton(CCObject*)
{
	auto& rlm = RouletteManager::get();

	m_integer_input_layer = RLIntegerInputAlertLayer::create({
		"List ID", "ID", 0, 0, INT_MAX,
		rlm.gdListID == 0 ? std::nullopt : std::optional<int>(rlm.gdListID),
		10, false,
		[&](auto iil) {
			rlm.gdListID = iil->m_integer;
			Mod::get()->setSavedValue<uint64_t>(
				DataManager::values::getKeyString<DMMiscKey::GD_LIST_ID>(),
				static_cast<uint64_t>(iil->m_integer)
			);
		}
	});
	if (m_integer_input_layer)
		m_integer_input_layer->show();
}


CCMenuItemToggler* RLRouletteInfoAlertLayer::createToggler(int tag, const std::string& nodeID, const char* labelText, CCPoint point, bool visible)
{
	auto buttonSpriteOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
	auto buttonSpriteOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
	buttonSpriteOn->setScale(.8f);
	buttonSpriteOff->setScale(.8f);

	auto button = CCMenuItemToggler::create(
		buttonSpriteOff,
		buttonSpriteOn,
		this,
		menu_selector(RLRouletteInfoAlertLayer::onToggleButton)
	);
	button->setPosition(point);
	button->setSizeMult(1.2f);
	button->setTag(tag);
	button->setVisible(visible);
	button->toggle(DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().at<bool>(tag));
	button->setID(nodeID);
	m_buttonMenu->addChild(button);

	auto label = CCLabelBMFont::create(labelText, "bigFont.fnt");
	label->setAnchorPoint({ .0f, .5f });
	label->limitLabelWidth(80.f, .5f, 0);
	label->setScale(.5f);
	label->setPosition({ point.x + 20.f, point.y });
	label->setID(nodeID + "-label");
	m_buttonMenu->addChild(label);

	auto infoIconSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
	infoIconSprite->setScale(.5f);
	auto infoIconButton = CCMenuItemSpriteExtra::create(
		infoIconSprite,
		this,
		menu_selector(RLRouletteInfoAlertLayer::onInfoIcon)
	);
	infoIconButton->setPosition({ point.x - 18.f, point.y + 16.f });
	infoIconButton->setID(nodeID + "-info");
	m_buttonMenu->addChild(infoIconButton);

	return button;
}
