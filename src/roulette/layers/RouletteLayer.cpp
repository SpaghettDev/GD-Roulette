#include <matjson.hpp>
#include <matjson/stl_serialize.hpp>
#include <thread>
#include "RouletteLayer.hpp"
#include "RouletteInfoLayer.hpp"
#include "../manager/RouletteManager.hpp"
#include "../../custom_layers/ConfirmationLayer.hpp"
#include "../../utils.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;


matjson::Value RouletteLayer::m_level;
ListFetcher RouletteLayer::m_list_fetcher;
std::string RouletteLayer::m_list_fetcher_error;

RouletteLayer* RouletteLayer::create()
{
	auto ret = new RouletteLayer();
	m_list_fetcher.init();

	if (ret && ret->init())
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RouletteLayer::init()
{
	if (!this->createBasics({ 440.f, 290.f }, menu_selector(RouletteLayer::onClose))) return false;

	m_pMainMenu = CCMenu::create();
	m_pPlayingMenu = CCMenu::create();
	m_pFinishedMenu = CCMenu::create();
	m_pErrorMenu = CCMenu::create();

	m_buttonMenu->setID("button-menu");
	m_pMainMenu->setID("main-menu");

	m_pPlayingMenu->setID("playing-menu");
	m_pPlayingMenu->setVisible(false);

	m_pFinishedMenu->setID("finished-menu");
	m_pFinishedMenu->setVisible(false);

	m_pErrorMenu->setID("error-menu");
	m_pErrorMenu->setVisible(false);

	m_mainLayer->addChild(m_pMainMenu, 11);
	m_mainLayer->addChild(m_pPlayingMenu, 11);
	m_mainLayer->addChild(m_pFinishedMenu, 11);
	m_mainLayer->addChild(m_pErrorMenu, 11);


	auto title = CCLabelBMFont::create("GD Level Roulette", "goldFont.fnt");
	title->setPosition({ .0f, 120.f });
	title->limitLabelWidth(340, 1, 0);
	m_buttonMenu->addChild(title);


	auto rouletteBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { .0f, .0f, 80.f, 80.f });
	rouletteBg->setContentSize({ 340.f, 210.f });
	rouletteBg->setAnchorPoint({ .5f, 1.f });
	rouletteBg->setColor({ 123, 60, 31 });
	rouletteBg->setPosition({ .0f, 95.f });
	m_buttonMenu->addChild(rouletteBg, -1);

	createButton("GJ_infoIcon_001.png", { 200.f, 125.f }, menu_selector(RouletteLayer::onInfoButton));
	auto optionsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsTxt_001.png");
	optionsSprite->setPosition({ 155.f, 110.f });
	optionsSprite->setVisible(!Mod::get()->setSavedValue<bool>("show-options-sprite", true));
	optionsSprite->setTag(1);
	m_buttonMenu->addChild(optionsSprite);

	createDifficultyButton(
		0, CCSprite::createWithSpriteFrameName("difficulty_01_btn_001.png"),
		{ -90.f, 40.f }, 1.2f
	);
	createDifficultyButton(
		1, CCSprite::createWithSpriteFrameName("difficulty_02_btn_001.png"),
		{ .0f, 40.f }, 1.2f
	);
	createDifficultyButton(
		2, CCSprite::createWithSpriteFrameName("difficulty_03_btn_001.png"),
		{ 90.f, 40.f }, 1.2f
	);
	createDifficultyButton(
		3, CCSprite::createWithSpriteFrameName("difficulty_04_btn_001.png"),
		{ -90.f, -25.f }, 1.2f
	);
	createDifficultyButton(
		4, CCSprite::createWithSpriteFrameName("difficulty_05_btn_001.png"),
		{ .0f, -25.f }, 1.2f
	);
	createDifficultyButton(
		5, CCSprite::createWithSpriteFrameName("difficulty_06_btn_001.png"),
		{ 90.f, -25.f }, 1.2f
	);
	createDifficultyButton(
		6, CCSprite::createWithSpriteFrameName("difficulty_07_btn2_001.png"),
		{ -120.f, -40.f }, 1.f, true, false
	);
	createDifficultyButton(
		7, CCSprite::createWithSpriteFrameName("difficulty_08_btn2_001.png"),
		{ -60.f, -40.f }, 1.f, true, false
	);
	createDifficultyButton(
		8, CCSprite::createWithSpriteFrameName("difficulty_06_btn2_001.png"),
		{ .0f, -40.f }, 1.f, true, false
	);
	createDifficultyButton(
		9, CCSprite::createWithSpriteFrameName("difficulty_09_btn2_001.png"),
		{ 60.f, -40.f }, 1.f, true, false
	);
	createDifficultyButton(
		10, CCSprite::createWithSpriteFrameName("difficulty_10_btn2_001.png"),
		{ 120.f, -40.f }, 1.f, true, false
	);
	auto plusButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
		this,
		menu_selector(RouletteLayer::onPlusButton)
	);
	plusButton->setPosition({ 135.f, -20.f });
	plusButton->setSizeMult(1.2f);
	plusButton->setTag(11);
	plusButton->setVisible(g_rouletteManager.getArrayState(g_rouletteManager.getFromSaveContainer("difficulty-array"), 5));
	m_pMainMenu->addChild(plusButton);

	auto startButtonText = CCLabelBMFont::create("Start", "bigFont.fnt");
	startButtonText->setPosition({ 46.f, 18.f });
	startButtonText->setScale(.725f);
	auto startButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_longBtn02_001.png"),
		this,
		menu_selector(RouletteLayer::onStartButton)
	);
	startButton->setPosition({ .0f, -85.f });
	startButton->setTag(12);
	startButton->addChild(startButtonText);
	m_pMainMenu->addChild(startButton);


	m_loading_circle = LoadingCircle::create();
	m_loading_circle->setPosition({ -285.f, -170.f });
	m_loading_circle->setVisible(false);
	m_buttonMenu->addChild(m_loading_circle);


	auto levelNameItemLabel = CCLabelBMFont::create("LevelName", "bigFont.fnt");
	levelNameItemLabel->limitLabelWidth(140.f, 1.f, .3f);
	auto levelNameItem = CCMenuItemSpriteExtra::create(
		levelNameItemLabel,
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelNameItem->setPosition({ 45.f, 75.f });
	levelNameItem->setTag(1);
	m_pPlayingMenu->addChild(levelNameItem);

	auto levelCreatorItemLabel = CCLabelBMFont::create("LevelCreator", "bigFont.fnt");
	levelCreatorItemLabel->limitLabelWidth(140.f, .8f, .2f);
	auto levelCreatorItem = CCMenuItemSpriteExtra::create(
		levelCreatorItemLabel,
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelCreatorItem->setPosition({ 45.f, 45.f });
	levelCreatorItem->setTag(2);
	m_pPlayingMenu->addChild(levelCreatorItem);

	auto levelIDItem = CCMenuItemSpriteExtra::create(
		CCLabelBMFont::create("LevelID", "bigFont.fnt"),
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelIDItem->setPosition({ 45.f, 17.f });
	levelIDItem->setScale(.5f);
	levelIDItem->setTag(3);
	m_pPlayingMenu->addChild(levelIDItem);

	for (int i = 1; i < 6; i++)
	{
		auto difficultySprite = CCSprite::createWithSpriteFrameName(fmt::format("difficulty_{0:0{1}}_btn_001.png", i, 2).c_str());
		difficultySprite->setPosition({ -110.f, static_cast<float>(i + 80) });
		difficultySprite->setScale(1.5f);
		difficultySprite->setVisible(false);
		difficultySprite->setTag(i + 3);

		m_pPlayingMenu->addChild(difficultySprite);
	}

	for (int i = 6; i < 11; i++)
	{
		int tag;

		auto demonDifficultySprite = CCSprite::createWithSpriteFrameName(fmt::format("difficulty_{0:0{1}}_btn2_001.png", i, 2).c_str());
		demonDifficultySprite->setPosition({ -110.f, static_cast<float>(i + 80) });
		demonDifficultySprite->setScale(1.5f);
		demonDifficultySprite->setVisible(false);
		// wtf
		switch (i)
		{
		case 6: tag = 9; break;
		case 7: tag = 12; break;
		case 8: tag = 13; break;
		case 9: tag = 10; break;
		case 10: tag = 11; break;
		}
		demonDifficultySprite->setTag(tag);
		m_pPlayingMenu->addChild(demonDifficultySprite);
	}

	auto levelPlayButtonItem = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png"),
		this,
		menu_selector(RouletteLayer::onPlayButton)
	);
	levelPlayButtonItem->setPosition({ -110.f, -53.f });
	levelPlayButtonItem->setTag(14);
	m_pPlayingMenu->addChild(levelPlayButtonItem);

	for (int i = 0; i < 3; i++)
	{
		auto levelCoinSprite = CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");
		levelCoinSprite->setPosition({ -120.f + 10.f * i, 13.f });
		levelCoinSprite->setScale(1.5f);
		levelCoinSprite->setVisible(false);
		levelCoinSprite->setTag(i + 15);
		if (i == 0)
			levelCoinSprite->setZOrder(-1);
		m_pPlayingMenu->addChild(levelCoinSprite);
	}

	auto epicSprite = CCSprite::createWithSpriteFrameName("GJ_epicCoin_001.png");
	epicSprite->setScale(1.4f);
	epicSprite->setPosition({ -110.f, 87.f });
	epicSprite->setVisible(false);
	epicSprite->setTag(18);
	m_pPlayingMenu->addChild(epicSprite, -1);

	auto featuredSprite = CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");
	featuredSprite->setScale(1.5f);
	featuredSprite->setPosition({ -110.f, 88.f });
	featuredSprite->setVisible(false);
	featuredSprite->setTag(19);
	m_pPlayingMenu->addChild(featuredSprite, -1);

	auto percentageText = CCLabelBMFont::create(
		fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str(),
		"goldFont.fnt"
	);
	percentageText->setPosition({ 50.f, -20.f });
	percentageText->setScale(.8f);
	percentageText->setTag(20);
	m_pPlayingMenu->addChild(percentageText);

	auto percentageBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
	percentageBg->setPosition({ 20.f, -35.f });
	percentageBg->setContentSize({ 60.f, 30.f });
	percentageBg->setAnchorPoint({ .0f, .0f });
	percentageBg->setColor({ 0, 0, 0 });
	percentageBg->setOpacity(100);
	percentageBg->setTag(21);
	m_pPlayingMenu->addChild(percentageBg, -1);

	auto skipButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Skip", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, 1.f),
		this,
		menu_selector(RouletteLayer::onSkipButton)
	);
	skipButton->setPosition({ -20.f, -70.f });
	skipButton->setTag(22);
	m_pPlayingMenu->addChild(skipButton);

	auto nextButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Next", 50, true, "bigFont.fnt", "GJ_button_01.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onNextButton)
	);
	nextButton->setPosition({ 50.f, -70.f });
	nextButton->setTag(23);
	m_pPlayingMenu->addChild(nextButton);

	auto resetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onResetButton)
	);
	resetButton->setPosition({ 120.f, -70.f });
	resetButton->setTag(24);
	m_pPlayingMenu->addChild(resetButton);


	auto ggText = CCLabelBMFont::create("GG!", "goldFont.fnt");
	ggText->setPosition({ .0f, 60.f });
	ggText->setTag(1);
	m_pFinishedMenu->addChild(ggText);

	auto ggSkipsUsedText = CCLabelBMFont::create("Skips Used: ", "bigFont.fnt");
	ggSkipsUsedText->setPosition({ .0f, 20.f });
	ggSkipsUsedText->setTag(2);
	m_pFinishedMenu->addChild(ggSkipsUsedText);

	auto ggNumLevelsText = CCLabelBMFont::create("Levels Played: ", "bigFont.fnt");
	ggNumLevelsText->setPosition({ .0f, -10.f });
	ggNumLevelsText->setTag(3);
	m_pFinishedMenu->addChild(ggNumLevelsText);

	auto finishedResetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onResetButton)
	);
	finishedResetButton->setPosition({ .0f, -70.f });
	finishedResetButton->setTag(4);
	m_pFinishedMenu->addChild(finishedResetButton);


	auto errorText = CCLabelBMFont::create("An error has occured", "bigFont.fnt");
	errorText->setPosition({ .0f, 10.f });
	errorText->setColor({ 255, 0, 0 });
	errorText->setScale(.8f);
	errorText->setTag(1);
	m_pErrorMenu->addChild(errorText);

	auto errorReasonText = CCLabelBMFont::create("ErrorReason", "bigFont.fnt");
	errorReasonText->setPosition({ .0f, -20.f });
	errorReasonText->limitLabelWidth(100.f, .6f, .3f);
	errorReasonText->setTag(2);
	m_pErrorMenu->addChild(errorReasonText);

	auto errorResetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onResetButton)
	);
	errorResetButton->setPosition({ 120.f, -70.f });
	errorResetButton->setTag(3);
	m_pErrorMenu->addChild(errorResetButton);


	if (g_rouletteManager.getArrayState(g_rouletteManager.getFromSaveContainer("difficulty-array"), 5))
	{
		if (RouletteLayer::m_plus_button_state)
		{
			RouletteLayer::m_plus_button_state = false;
			RouletteLayer::onPlusButton(nullptr);
		}

		for (int i = 6; i < 11; i++)
		{
			if (g_rouletteManager.getArrayState(g_rouletteManager.getFromSaveContainer("demon-difficulty-array"), i - 6))
				as<CCMenuItemSpriteExtra*>(
					m_pMainMenu->getChildByTag(i)
				)->setColor({ 255, 255, 255 });
		}
	}


	return true;
}

// called every frame after the ListFetcher thread has been detatched
void RouletteLayer::update(float)
{
	if (!m_list_fetcher.isFetching)
	{
		finishLevelRoulette();
		this->unscheduleUpdate();
	}
}

void RouletteLayer::onClose(CCObject*)
{
	if (g_rouletteManager.isPlayingRoulette)
	{
		m_confirmation_layer->setup({
			"Woah there!",
			"Are you sure you want to quit the roulette?",
			[&](auto cl) {
				g_rouletteManager.reset();

				setKeypadEnabled(false);
				removeFromParentAndCleanup(true);
				g_rouletteManager.rouletteLayer = nullptr;
			},
			[&](auto cl) {}
		});

		m_confirmation_layer = ConfirmationAlertLayer::create();
		if (m_confirmation_layer)
			m_confirmation_layer->show();
	}
	else
	{
		setKeypadEnabled(false);
		removeFromParentAndCleanup(true);
		g_rouletteManager.rouletteLayer = nullptr;
	}
}

void RouletteLayer::onInfoButton(CCObject*)
{
	if (!g_rouletteManager.isPlayingRoulette)
	{
		if (Mod::get()->getSavedValue<bool>("show-options-sprite"))
			m_buttonMenu->getChildByTag(1)->setVisible(false);

		RouletteInfoLayer::create()->show();
	}
	else
		// this->addChild(TextAlertPopup::create("You are currently in a game of roulette!", 1.2f, .8f, 1, "bigFont.fnt"));
		Notification::create("You are currently in a game of roulette!", NotificationIcon::Error)->show();
}

// TODO: split to onDemonDifficultyChosen and onDifficultyChosen
void RouletteLayer::onDifficultyChosen(CCObject* sender)
{
	if (
		auto ind = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("selected-list-array").as_array(), true);
		ind != 0
		)
		return;

	auto tag = sender->getTag();
	auto difficultyButton = as<CCMenuItemSpriteExtra*>(sender);

	// check if difficultyButton is one of the demon types and not a regular difficulty
	if (tag > 5 && (tag < 10 || tag > 5))
	{
		int ind = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array(), true);
		as<CCMenuItemSpriteExtra*>(m_pMainMenu->getChildByTag(ind + 6))->setColor({ 125, 125, 125 });

		g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array().at(ind) = false;
		g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array().at(tag - 6) = true;
	}
	else
	{
		if (m_plus_button_state && tag != 5)
			onPlusButton(nullptr);

		int ind = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("difficulty-array").as_array(), true);
		as<CCMenuItemSpriteExtra*>(m_pMainMenu->getChildByTag(ind))->setColor({ 125, 125, 125 });

		g_rouletteManager.getFromSaveContainer("difficulty-array").as_array().at(ind) = false;
		g_rouletteManager.getFromSaveContainer("difficulty-array").as_array().at(tag) = true;
		g_rouletteManager.previousDifficulty = tag;
	}

	difficultyButton->setColor({ 255, 255, 255 });

	// demon
	m_pMainMenu->getChildByTag(11)->setVisible(g_rouletteManager.getArrayState(g_rouletteManager.getFromSaveContainer("difficulty-array"), 5));
}

void RouletteLayer::onStartButton(CCObject*)
{
	m_pMainMenu->setVisible(false);

	int diffInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("difficulty-array").as_array(), true);
	int demonInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array(), true);
	int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

	getRandomListLevel(difficulty, m_level, m_list_fetcher_error);

	onNextLevel(false, true);

	g_rouletteManager.isPlayingRoulette = true;
}

void RouletteLayer::onPlusButton(CCObject*)
{
	m_plus_button_state = !m_plus_button_state;

	if (m_plus_button_state)
	{
		for (int i = 0; i < 6; i++)
			m_pMainMenu->getChildByTag(i)->setPositionY(i < 3 ? 64.f : 10.f);

		m_pMainMenu->getChildByTag(11)->setPositionY(15.f);
		m_pMainMenu->getChildByTag(12)->setPositionY(-90.f);
	}
	else
	{
		for (int i = 0; i < 6; i++)
			m_pMainMenu->getChildByTag(i)->setPositionY(i < 3 ? 40.f : -25.f);

		m_pMainMenu->getChildByTag(11)->setPositionY(-20.f);
		m_pMainMenu->getChildByTag(12)->setPositionY(-85.f);
	}

	for (int i = 6; i < 11; i++)
		m_pMainMenu->getChildByTag(i)->setVisible(m_plus_button_state);
}

void RouletteLayer::onLevelInfo(CCObject* sender)
{
	auto textButton = as<CCMenuItemSpriteExtra*>(sender);
	std::string text;

	switch (textButton->getTag())
	{
	case 100:
		text = m_level.get<std::string>("name");
		if (text.size() > 15)
			textButton->setScale(.75f);
		break;
	case 101:
		textButton->setScale(.8f);
		text = m_level.get<std::string>("author");
		break;
	case 102:
		textButton->setScale(.5f);
		text = m_level.get<std::string>("id");
		break;
	}

	clipboard::write(text);
	// this->addChild(TextAlertPopup::create("Copied to Clipboard", .5f, .6f, 1, "bigFont.fnt"));
	Notification::create("Copied to Clipboard", NotificationIcon::Success)->show();
}

void RouletteLayer::onPlayButton(CCObject*)
{
	if (m_list_fetcher.isFetching)
		return;

	auto level = GJGameLevel::create();
	level->m_levelID = std::stoi(m_level.get<std::string>("id"));
	level->m_levelName = m_level.get<std::string>("name");
	level->m_creatorName = m_level.get<std::string>("author");
	level->m_accountID = std::stoi(m_level.get<std::string>("accountID"));

	if (m_level["songID"].is_number())
		level->m_songID = m_level.get<int>("songID");
	else
		// song is not a custom one, returns "Level 1", remove "Level "
		level->m_audioTrack = std::stoi(m_level.get<std::string>("songID").substr(6));

	level->m_levelNotDownloaded = true;
	level->m_levelType = GJLevelType::Saved;

	auto layer = LevelInfoLayer::create(level, false);
	layer->downloadLevel();
	auto scene = CCScene::create();
	scene->addChild(layer);
	CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

void RouletteLayer::onNextButton(CCObject*)
{
	if (m_list_fetcher.isFetching)
		return;

	if (g_rouletteManager.currentLevelPercentage == 100)
	{
		onNextLevel(false, false);

		as<CCLabelBMFont*>(m_pFinishedMenu->getChildByTag(2))->setString(
			fmt::format("Skips Used: {}", g_rouletteManager.skipsUsed).c_str()
		);
		as<CCLabelBMFont*>(m_pFinishedMenu->getChildByTag(3))->setString(
			fmt::format("Levels Played: {}", g_rouletteManager.numLevels).c_str()
		);

		m_pPlayingMenu->setVisible(false);
		m_pFinishedMenu->setVisible(true);
	}
	else if (g_rouletteManager.currentLevelPercentage != 0 && g_rouletteManager.hasFinishedPreviousLevel)
	{
		g_rouletteManager.hasFinishedPreviousLevel = false;

		onNextLevel(false, true, -125.f);

		as<CCLabelBMFont*>(
			m_pPlayingMenu->getChildByTag(20)
		)->setString(fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str());

		int diffInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("difficulty-array").as_array(), true);
		int demonInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array(), true);
		int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

		getRandomListLevel(difficulty, m_level, m_list_fetcher_error);
	}
	else
	{
		// this->addChild(TextAlertPopup::create(
		// 	std::format(
		// 		"You need to get at least {}%!", static_cast<int>(g_rouletteManager.levelPercentageGoal)
		// 	), 1.2f, .8f, 1, "bigFont.fnt"
		// ));
		Notification::create(
			fmt::format("You need to get at least {}%!", g_rouletteManager.levelPercentageGoal),
			NotificationIcon::Error
		)->show();
	}
}

void RouletteLayer::onResetButton(CCObject*)
{
	if (m_list_fetcher.isFetching)
		return;

	g_rouletteManager.reset();

	if (!m_level.is_null())
		m_level.as_object().clear();

	// 
	/*for (int i = 0; i < 6; i++)
		m_pMainMenu->getChildByTag(i)->setPositionY(i < 3 ? 40.f : -25.f);*/

	onNextLevel();

	m_pMainMenu->setVisible(true);
	m_pPlayingMenu->setVisible(false);
	m_pFinishedMenu->setVisible(false);
	m_pErrorMenu->setVisible(false);

	as<CCLabelBMFont*>(m_pPlayingMenu->getChildByTag(20))->setString(
		fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str()
	);

	m_pMainMenu->getChildByTag(11)->setPositionY(-20.f);
	m_pMainMenu->getChildByTag(12)->setVisible(true);
	m_pMainMenu->getChildByTag(12)->setPositionY(-85.f);

	if (g_rouletteManager.getArrayState(g_rouletteManager.getFromSaveContainer("difficulty-array"), 5))
	{
		m_plus_button_state = true;
		onPlusButton(nullptr);
	}

	m_loading_circle->setPositionY(-170.f);
}

void RouletteLayer::onSkipButton(CCObject*)
{
	if (m_list_fetcher.isFetching)
		return;

	if (g_rouletteManager.currentLevelPercentage == 100)
	{
		onNextButton(nullptr);
		return;
	}

	if (g_rouletteManager.skipsUsed < Mod::get()->getSettingValue<int64_t>("max-skips"))
	{
		g_rouletteManager.skipsUsed++;
		g_rouletteManager.hasFinishedPreviousLevel = false;

		onNextLevel(false, true, -125.f);

		int diffInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("difficulty-array").as_array(), true);
		int demonInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array(), true);
		int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

		getRandomListLevel(difficulty, m_level, m_list_fetcher_error);
	}
	else
	{
		// this->addChild(
		// 	TextAlertPopup::create(
		// 		"You don't have any skips left!", 1.2f, .8f, 1, "bigFont.fnt"
		// 	)
		// );
		Notification::create("You don't have any skips left!", NotificationIcon::Error)->show();
	}
}

void RouletteLayer::onEnter()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority, true);
	CCLayer::onEnter();
}

void RouletteLayer::finishLevelRoulette()
{
	m_loading_circle->stopAllActions();
	m_loading_circle->setVisible(false);

	if (m_level.is_null())
	{
		as<CCLabelBMFont*>(m_pErrorMenu->getChildByTag(2))->setString(m_list_fetcher_error.c_str());

		m_pPlayingMenu->setVisible(false);
		m_pErrorMenu->setVisible(true);
		g_rouletteManager.isPlayingRoulette = false;
		return;
	}

	onNextLevel(true);

	g_rouletteManager.currentLevelID = std::stoi(m_level.get<std::string>("id"));

	as<CCLabelBMFont*>(
		m_pPlayingMenu->getChildByTag(1)->getChildren()->objectAtIndex(0)
	)->setString(m_level.get<std::string>("name").c_str());
	as<CCLabelBMFont*>(
		m_pPlayingMenu->getChildByTag(2)->getChildren()->objectAtIndex(0)
	)->setString(("by " + m_level.get<std::string>("author")).c_str());
	as<CCLabelBMFont*>(
		m_pPlayingMenu->getChildByTag(3)->getChildren()->objectAtIndex(0)
	)->setString(("ID: " + m_level.get<std::string>("id")).c_str());

	auto diffInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("difficulty-array").as_array(), true);
	auto demonInd = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array(), true);
	int difficultyTag = m_difficulty_to_tag.at(m_level.get<std::string>("difficulty"));

	if (int coins = m_level.get<int>("coins"); coins > 0)
	{
		m_pPlayingMenu->getChildByTag(difficultyTag)->setPositionY(55.f);

		for (int i = 0; i < 3; i++)
			m_pPlayingMenu->getChildByTag(i + 15)->setPosition({ -120.f + 10.f * i, difficultyTag > 8 ? 13.f : 10.f });

		switch (coins)
		{
		case 1:
			m_pPlayingMenu->getChildByTag(16)->setVisible(true);
			if (diffInd == 5)
				m_pPlayingMenu->getChildByTag(16)->setPositionY(.0f);
			break;
		case 2:
			m_pPlayingMenu->getChildByTag(16)->setVisible(true);
			m_pPlayingMenu->getChildByTag(17)->setVisible(true);

			m_pPlayingMenu->getChildByTag(16)->setPositionX(-115.f);
			m_pPlayingMenu->getChildByTag(17)->setPositionX(-105.f);

			if (diffInd == 5)
			{
				m_pPlayingMenu->getChildByTag(16)->setPositionY(.0f);
				m_pPlayingMenu->getChildByTag(17)->setPositionY(.0f);
			}
			break;
		case 3:
			for (int i = 0; i < coins; i++)
			{
				m_pPlayingMenu->getChildByTag(i + 15)->setVisible(true);
				if (diffInd == 5)
					m_pPlayingMenu->getChildByTag(i + 15)->setPositionY(.0f);
			}
			break;
		}
	}
	else
		m_pPlayingMenu->getChildByTag(difficultyTag)->setPositionY(40.f);

	m_pPlayingMenu->getChildByTag(difficultyTag)->setVisible(true);

	if (m_level.get<bool>("epic"))
	{
		m_pPlayingMenu->getChildByTag(18)->setPosition(
			m_pPlayingMenu->getChildByTag(difficultyTag)->getPosition()
		);
		m_pPlayingMenu->getChildByTag(18)->setVisible(true);
	}

	if (m_level.get<bool>("featured"))
	{
		m_pPlayingMenu->getChildByTag(19)->setPosition(
			m_pPlayingMenu->getChildByTag(difficultyTag)->getPosition()
		);
		m_pPlayingMenu->getChildByTag(19)->setVisible(true);
	}

	m_pMainMenu->setVisible(false);
	m_pPlayingMenu->setVisible(true);
}

void RouletteLayer::onNextLevel(bool levelTextVisible, bool enableLoadingCircle, float loadingCirclePosY)
{
	// sets PlayingMenu's difficulty faces, featured/epic glow, coins and level text visibility
	for (int i = 0; i < 16; i++)
	{
		if (i == 10) continue;
		m_pPlayingMenu->getChildByTag(i + 4)->setVisible(false);
	}
	for (int i = 0; i < 3; i++)
	{
		m_pPlayingMenu->getChildByTag(i + 1)->setVisible(levelTextVisible);
		m_pPlayingMenu->getChildByTag(i + 15)->setVisible(false);
	}

	m_loading_circle->setPositionY(loadingCirclePosY);
	if (enableLoadingCircle)
	{
		m_loading_circle->setVisible(true);
		m_loading_circle->runAction(CCRepeatForever::create(CCRotateBy::create(1.f, 360)));
	}
}


void RouletteLayer::getRandomListLevel(int difficulty, matjson::Value& list, std::string& error)
{
	int listType = roulette::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("selected-list-array").as_array(), true);
	std::thread getListThread;

	switch (listType)
	{
	case 0:
		getListThread = std::thread([&](int difficulty, matjson::Value& list, std::string& error) {
			m_list_fetcher.getRandomNormalListLevel(difficulty, std::ref(list), std::ref(error));
		}, difficulty, std::ref(list), std::ref(error));
		break;
	case 1:
		getListThread = std::thread([&](matjson::Value& list, std::string& error) {
			m_list_fetcher.getRandomDemonListLevel(std::ref(list), std::ref(error));
		}, std::ref(list), std::ref(error));
		break;
	case 2:
		getListThread = std::thread([&](matjson::Value& list, std::string& error) {
			m_list_fetcher.getRandomChallengeListLevel(std::ref(list), std::ref(error));
		}, std::ref(list), std::ref(error));
		break;
	default:
		m_level = matjson::parse("null");
		break;
	}

	// manually set to true because sometimes the thread detatches too late
	m_list_fetcher.isFetching = true;
	this->scheduleUpdate();
	getListThread.detach();
}

CCMenuItemSpriteExtra* RouletteLayer::createDifficultyButton(int tag, CCNode* sprite, CCPoint point, float scale, bool isDemon, bool visible)
{
	sprite->setScale(scale);
	auto button = CCMenuItemSpriteExtra::create(
		sprite,
		this,
		menu_selector(RouletteLayer::onDifficultyChosen)
	);
	button->setPosition(point);
	button->setTag(tag);
	if (isDemon)
	{
		if (!g_rouletteManager.getArrayState(g_rouletteManager.getFromSaveContainer("difficulty-array"), tag - 6))
			button->setColor({ 125, 125, 125 });
	}
	else
	{
		if (!g_rouletteManager.getArrayState(g_rouletteManager.getFromSaveContainer("difficulty-array"), tag))
			button->setColor({ 125, 125, 125 });
	}
	button->setVisible(visible);
	m_pMainMenu->addChild(button);

	return button;
}
