#include "RLRouletteInfoLayer.hpp"
#include "../manager/RouletteManager.hpp"
#include "../../custom_layers/RLIntegerInputLayer.hpp"
#include "../../custom_nodes/RLDifficultyNode.hpp"
#include "../../utils.hpp"

#include <matjson/stl_serialize.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

GJDifficulty RLRouletteInfoLayer::m_previous_roulette_difficulty = static_cast<GJDifficulty>(-3);

RLRouletteInfoLayer* RLRouletteInfoLayer::create()
{
	auto ret = new RLRouletteInfoLayer();

	if (ret && ret->init())
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLRouletteInfoLayer::init()
{
	if (!this->createBasics({ 365.f, 240.f }, menu_selector(RLRouletteInfoLayer::onClose))) return false;

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

	createToggler(0, "normal-list", "Normal List", { -120.f, 15.f });
	createToggler(1, "demon-list", "Demon List", { 20.f, 15.f });
	createToggler(2, "challenge-list", "Challenge List", { -120.f, -25.f });
	createToggler(3, "gd-list", "GD List", { 20.f, -25.f });

	auto idSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
	idSprite->setScale(.5f);
	auto idButton = CCMenuItemSpriteExtra::create(
		idSprite,
		this,
		menu_selector(RLRouletteInfoLayer::onIDButton)
	);
	idButton->setVisible(
		rl::utils::getIndexOf(
			g_rouletteManager.getFromSaveContainer("selected-list-array").as_array(), true
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
		menu_selector(RLRouletteInfoLayer::onNumSkipsButton)
	);
	skipsButton->setPosition({ .0f, -65.f });
	skipsButton->addChild(skipsButtonText);
	skipsButton->setID("skips-button");
	m_buttonMenu->addChild(skipsButton);


	auto versionText = CCLabelBMFont::create(
#if defined(RWDI_MODE) && defined(GEODE_PLATFORM_SHORT_IDENTIFIER)
		fmt::format("Version: {} ({} dev)", Mod::get()->getVersion().toString(true), GEODE_PLATFORM_SHORT_IDENTIFIER).c_str(),
#elif defined(GEODE_PLATFORM_SHORT_IDENTIFIER)
		fmt::format("Version: {} ({})", Mod::get()->getVersion().toString(true), GEODE_PLATFORM_SHORT_IDENTIFIER).c_str(),
#else
		fmt::format("Version: {} (how)", Mod::get()->getVersion().toString(true)).c_str(),
#endif
		"bigFont.fnt"
	);
	versionText->setPosition({ .0f, -94.f });
	versionText->setScale(.5f);
	m_buttonMenu->addChild(versionText);


	return true;
}

void RLRouletteInfoLayer::onClose(CCObject*)
{
	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void RLRouletteInfoLayer::onToggleButton(CCObject* sender)
{
	auto button = static_cast<CCMenuItemToggler*>(sender);
	auto prevIdx = rl::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("selected-list-array").as_array(), true);
	const auto demonDifficultyButton = static_cast<RLDifficultyNode*>(
		g_rouletteManager.rouletteLayer->getDifficultyButton(GJDifficulty::Demon)->getChildByID("sprite-node")
	);

	// yeah...
	auto changeListWrapper = [&](const std::function<void()>& f) {
		g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(button->getTag()) = false;
		g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(prevIdx) = false;
		g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(0) = true;

		f();

		g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(0) = false;
		g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(prevIdx) = false;
		g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(button->getTag()) = true;
	};

	g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(prevIdx) = false;
	g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(button->getTag()) = true;

	m_buttonMenu->getChildByID("list-id-button")->setVisible(button->getTag() == 3);

	for (int i = 0; i < 4; i++)
		static_cast<CCMenuItemToggler*>(m_buttonMenu->getChildByTag(i))->toggle(false);

	if (m_previous_roulette_difficulty == static_cast<GJDifficulty>(-3))
		m_previous_roulette_difficulty = g_rouletteManager.previousDifficulty;

	// purely visual, "toggles" the difficulty face based on the selected list
	// (demon for demon list, insane for challenge list, and previous difficulty for normal list, easy for gd list because 🔥)
	if (button->getID() == "normal-list")
	{
		changeListWrapper([&] {
			g_rouletteManager.rouletteLayer->onDifficultyButton(
				g_rouletteManager.rouletteLayer->getDifficultyButton(m_previous_roulette_difficulty)
			);
		});

		m_previous_roulette_difficulty = static_cast<GJDifficulty>(-3);
	}
	else if (button->getID() == "demon-list")
	{
		changeListWrapper([&] {
			g_rouletteManager.rouletteLayer->onDifficultyButton(
				g_rouletteManager.rouletteLayer->getDifficultyButton(GJDifficulty::Demon)
			);
		});
		demonDifficultyButton->setDifficulty(GJDifficulty::DemonExtreme);
	}
	else if (button->getID() == "challenge-list")
	{
		changeListWrapper([&] {
			g_rouletteManager.rouletteLayer->onDifficultyButton(
				g_rouletteManager.rouletteLayer->getDifficultyButton(GJDifficulty::Insane)
			);
		});
		demonDifficultyButton->setDifficulty(static_cast<GJDifficulty>(-2));
	}
	else if (button->getID() == "gd-list")
	{
		// TODO: set the list's difficulty ... somehow?

		changeListWrapper([&] {
			g_rouletteManager.rouletteLayer->onDifficultyButton(
				g_rouletteManager.rouletteLayer->getDifficultyButton(GJDifficulty::Easy)
			);
			g_rouletteManager.rouletteLayer->getDifficultyButton(GJDifficulty::Easy)->setColor({ 125, 125, 125 });
		});
		demonDifficultyButton->setDifficulty(static_cast<GJDifficulty>(-2));
	}

	if (button->getID() != "normal-list")
		g_rouletteManager.rouletteLayer->main_menu->getChildByID("demon-plus-button")->setVisible(false);
	else if (m_previous_roulette_difficulty == GJDifficulty::Demon)
		g_rouletteManager.rouletteLayer->main_menu->getChildByID("demon-plus-button")->setVisible(true);

	if (button->getID() == "normal-list")
		demonDifficultyButton->setDifficulty(
			m_previous_roulette_difficulty == GJDifficulty::Demon
				? g_rouletteManager.previousDemonDifficulty
				: static_cast<GJDifficulty>(-2)
			);
}

void RLRouletteInfoLayer::onNumSkipsButton(CCObject*)
{
	m_integer_input_layer = RLIntegerInputLayer::create({
		"Number Of Skips", "Skips", 0, 3, 9999,
		static_cast<int>(Mod::get()->getSettingValue<int64_t>("max-skips")), true,
		[&](auto iil) {
			Mod::get()->setSettingValue<int64_t>("max-skips", iil->m_integer);
		}
	});
	if (m_integer_input_layer)
		m_integer_input_layer->show();
}

void RLRouletteInfoLayer::onInfoIcon(CCObject* sender)
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
			"A GD List.\nPretty self-explanatory.\n<cr>Platformer levels appear in this list!</c> <cy>No</c>, i will not fix :D",
			"OK",
			nullptr
		)->show();
	}
}

void RLRouletteInfoLayer::onIDButton(CCObject*)
{
	m_integer_input_layer = RLIntegerInputLayer::create({
		"List ID", "ID", 0, 0, INT_MAX,
		g_rouletteManager.gdListID == 0 ? std::nullopt : std::optional<int>(g_rouletteManager.gdListID), false,
		[&](auto iil) {
			g_rouletteManager.gdListID = iil->m_integer;
			Mod::get()->setSavedValue<uint64_t>("gdListID", static_cast<uint64_t>(iil->m_integer));
		}
	});
	if (m_integer_input_layer)
		m_integer_input_layer->show();
}


CCMenuItemToggler* RLRouletteInfoLayer::createToggler(int tag, const std::string& nodeID, const char* labelText, CCPoint point, bool visible)
{
	auto buttonSpriteOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
	auto buttonSpriteOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
	buttonSpriteOn->setScale(.8f);
	buttonSpriteOff->setScale(.8f);

	auto button = CCMenuItemToggler::create(
		buttonSpriteOff,
		buttonSpriteOn,
		this,
		menu_selector(RLRouletteInfoLayer::onToggleButton)
	);
	button->setPosition(point);
	button->setSizeMult(1.2f);
	button->setTag(tag);
	button->setVisible(visible);
	button->toggle(g_rouletteManager.getFromSaveContainer("selected-list-array").as_array().at(tag).as<bool>());
	button->setID(nodeID);
	m_buttonMenu->addChild(button);

	auto label = rl::utils::createTextLabel(labelText, { point.x + 20.f, point.y }, .5f, m_buttonMenu);
	label->setAnchorPoint({ .0f, .5f });
	label->limitLabelWidth(80.f, .5f, 0);
	label->setID(nodeID + "-label");

	auto infoIconSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
	infoIconSprite->setScale(.5f);
	auto infoIconButton = CCMenuItemSpriteExtra::create(
		infoIconSprite,
		this,
		menu_selector(RLRouletteInfoLayer::onInfoIcon)
	);
	infoIconButton->setPosition({ point.x - 18.f, point.y + 16.f });
	infoIconButton->setID(nodeID + "-info");
	m_buttonMenu->addChild(infoIconButton);

	return button;
}
