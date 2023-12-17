#include <nlohmann/json.hpp>
#include <thread>
#include "RouletteLayer.hpp"
#include "RouletteInfoLayer.hpp"
#include "../../custom_layers/ConfirmationLayer.hpp"
#include "../../utils.hpp"

#define INITIALIZEROULETTEMANAGER
#include "../manager/RouletteManager.hpp"


nlohmann::json RouletteLayer::m_level;
ListFetcher RouletteLayer::m_list_fetcher;
curlResponse RouletteLayer::m_list_fetcher_response;

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

	m_pPlayingMenu->setVisible(false);
	m_pFinishedMenu->setVisible(false);
	m_pErrorMenu->setVisible(false);

	m_pLayer->addChild(m_pMainMenu, 11);
	m_pLayer->addChild(m_pPlayingMenu, 11);	
	m_pLayer->addChild(m_pFinishedMenu, 11);
	m_pLayer->addChild(m_pErrorMenu, 11);


	auto title = CCLabelBMFont::create("GD Level Roulette", "goldFont.fnt");
	title->setPosition({ .0f, 120.f });
	title->limitLabelWidth(340, 1, 0);
	m_pButtonMenu->addChild(title);


	auto rouletteBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { .0f, .0f, 80.f, 80.f });
	rouletteBg->setContentSize({ 340.f, 210.f });
	rouletteBg->setAnchorPoint({ .5f, 1.f });
	rouletteBg->setColor({ 123, 60, 31 });
	rouletteBg->setPosition({ .0f, 95.f });
	m_pButtonMenu->addChild(rouletteBg, -1);

	createButton("GJ_infoIcon_001.png", { 200.f, 125.f }, menu_selector(RouletteLayer::onInfoButton));
	auto optionsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsTxt_001.png");
	optionsSprite->setPosition({ 155.f, 110.f });
	optionsSprite->setVisible(RouletteManager.showOptionsSprite);
	optionsSprite->setTag(1);
	m_pButtonMenu->addChild(optionsSprite);

	
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
	auto plusButton = gd::CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
		this,
		menu_selector(RouletteLayer::onPlusButton)
	);
	plusButton->setPosition({ 135.f, -20.f });
	plusButton->setSizeMult(1.2f);
	plusButton->setTag(11);
	plusButton->setVisible(RouletteManager.difficultyArr[5]);
	m_pMainMenu->addChild(plusButton);

	auto startButtonText = CCLabelBMFont::create("Start", "bigFont.fnt");
	startButtonText->setPosition({ 46.f, 16.f });
	startButtonText->setScale(.725f);
	auto startButton = gd::CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_longBtn02_001.png"),
		this,
		menu_selector(RouletteLayer::onStartButton)
	);
	startButton->setPosition({ .0f, -85.f });
	startButton->setTag(12);
	startButton->addChild(startButtonText);
	m_pMainMenu->addChild(startButton);


	m_loading_circle = gd::LoadingCircle::create();
	m_loading_circle->setPosition({ -285.f, -170.f });
	m_loading_circle->setVisible(false);
	m_pButtonMenu->addChild(m_loading_circle);


	auto levelNameItem = gd::CCMenuItemSpriteExtra::create(
		CCLabelBMFont::create("LevelName", "bigFont.fnt"),
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelNameItem->setPosition({ 45.f, 75.f });
	levelNameItem->setTag(1);
	m_pPlayingMenu->addChild(levelNameItem);

	auto levelAuthorItem = gd::CCMenuItemSpriteExtra::create(
		CCLabelBMFont::create("LevelAuthor", "bigFont.fnt"),
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelAuthorItem->setPosition({ 45.f, 45.f });
	levelAuthorItem->setTag(2);
	levelAuthorItem->setScale(.8f);
	m_pPlayingMenu->addChild(levelAuthorItem);

	auto levelIDItem = gd::CCMenuItemSpriteExtra::create(
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
		auto difficultySprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("difficulty_%02u_btn_001.png", i)->getCString());
		difficultySprite->setPosition({ -110.f, static_cast<float>(i + 80) });
		difficultySprite->setScale(1.5f);
		difficultySprite->setVisible(false);
		difficultySprite->setTag(i + 3);

		m_pPlayingMenu->addChild(difficultySprite);
	}

	for (int i = 6; i < 11; i++)
	{
		int tag;

		auto demonDifficultySprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("difficulty_%02u_btn2_001.png", i)->getCString());
		demonDifficultySprite->setPosition({ -110.f, static_cast<float>(i + 80) });
		demonDifficultySprite->setScale(1.5f);
		demonDifficultySprite->setVisible(false);
		// wtf
		switch (i)
		{
		case 6: tag = 9; break; case 7: tag = 12; break;
		case 8: tag = 13; break; case 9: tag = 10; break;
		case 10: tag = 11; break;
		}
		demonDifficultySprite->setTag(tag);
		m_pPlayingMenu->addChild(demonDifficultySprite);
	}

	auto levelPlayButtonItem = gd::CCMenuItemSpriteExtra::create(
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
		CCString::createWithFormat("%d%%", RouletteManager.lastLevelPercentage)->getCString(), "goldFont.fnt"
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

	auto skipButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Skip", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, 1.f),
		this,
		menu_selector(RouletteLayer::onSkipButton)
	);
	skipButton->setPosition({ -20.f, -70.f });
	skipButton->setTag(22);
	m_pPlayingMenu->addChild(skipButton);

	auto nextButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Next", 50, true, "bigFont.fnt", "GJ_button_01.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onNextButton)
	);
	nextButton->setPosition({ 50.f, -70.f });
	nextButton->setTag(23);
	m_pPlayingMenu->addChild(nextButton);

	auto resetButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
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

	auto finishedResetButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onResetButton)
	);
	finishedResetButton->setPosition({ .0f, -70.f });
	finishedResetButton->setTag(4);
	m_pFinishedMenu->addChild(finishedResetButton);


	auto errorText = CCLabelBMFont::create("An error has occured", "bigFont.fnt");
	errorText->setPosition({ .0f, 10.f });
	errorText->setColor({ 255, 0, 0 });
	errorText->setTag(1);
	m_pErrorMenu->addChild(errorText);

	auto errorReasonText = CCLabelBMFont::create("Error Reason", "bigFont.fnt");
	errorReasonText->setPosition({ .0f, -20.f });
	errorReasonText->setScale(.6f);
	errorReasonText->setTag(2);
	m_pErrorMenu->addChild(errorReasonText);

	auto errorResetButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onResetButton)
	);
	errorResetButton->setPosition({ 120.f, -70.f });
	errorResetButton->setTag(3);
	m_pErrorMenu->addChild(errorResetButton);


	if (RouletteManager.difficultyArr[5])
	{
		if (RouletteLayer::m_plus_button_state)
		{
			RouletteLayer::m_plus_button_state = false;
			RouletteLayer::onPlusButton(nullptr);
		}

		for (int i = 6; i < 11; i++)
			if (RouletteManager.demonDifficultyArr[i - 6])
				reinterpret_cast<gd::CCMenuItemSpriteExtra*>(
					m_pMainMenu->getChildByTag(i)
				)->setColor({ 255, 255, 255 });
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
	if (RouletteManager.isPlayingRoulette)
	{
		m_confirmation_layer->setup({
			"Woah there!",
			"Are you sure you want to quit the roulette?",
			[&](auto cl) {
				RouletteManager.reset();

				setKeypadEnabled(false);
				removeFromParentAndCleanup(true);
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
	}
}

void RouletteLayer::onInfoButton(CCObject*)
{
	if (!RouletteManager.isPlayingRoulette)
	{
		if (RouletteManager.showOptionsSprite)
		{
			m_pButtonMenu->getChildByTag(1)->setVisible(false);
			RouletteManager.showOptionsSprite = false;
		}

		RouletteInfoLayer::create()->show();
	}
	else
		this->addChild(gd::TextAlertPopup::create("You are currently in a game of roulette!", 1.2f, .8f));
}

// TODO: split to onDemonDifficultyChosen and onDifficultyChosen
void RouletteLayer::onDifficultyChosen(CCObject* sender)
{
	if (
		auto ind = utils::getIndexOf(RouletteManager.selectedListArr, true);
		ind != 0
		)
		return;

	auto tag = sender->getTag();
	auto difficultyButton = reinterpret_cast<gd::CCMenuItemSpriteExtra*>(sender);
	auto& diffArr = RouletteManager.difficultyArr;
	auto& demonDiffArr = RouletteManager.demonDifficultyArr;

	// check if difficultyButton is one of the demon types and not a regular difficulty
	if (tag > 5 && (tag < 10 || tag > 5))
	{
		int ind = utils::getIndexOf(demonDiffArr, true);
		reinterpret_cast<gd::CCMenuItemSpriteExtra*>(
			m_pMainMenu->getChildByTag(ind + 6)
		)->setColor({ 125, 125, 125 });

		demonDiffArr.at(ind) = false;
		demonDiffArr.at(tag - 6) = true;
	}
	else
	{
		if (m_plus_button_state && tag != 5)
			onPlusButton(nullptr);

		int ind = utils::getIndexOf(diffArr, true);
		reinterpret_cast<gd::CCMenuItemSpriteExtra*>(
			m_pMainMenu->getChildByTag(ind)
		)->setColor({ 125, 125, 125 });

		diffArr.at(ind) = false;
		diffArr.at(tag) = true;
	}

	difficultyButton->setColor({ 255, 255, 255 });

	// demon
	m_pMainMenu->getChildByTag(11)->setVisible(diffArr[5]);
}

void RouletteLayer::onStartButton(CCObject*)
{
	m_pMainMenu->setVisible(false);

	int diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
	int demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
	int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

	getRandomListLevel(difficulty, m_level, m_list_fetcher_response);

	onNextLevel(false, true);

	RouletteManager.isPlayingRoulette = true;
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
	auto textButton = reinterpret_cast<gd::CCMenuItemSpriteExtra*>(sender);
	std::string text;

	switch (textButton->getTag())
	{
	case 100:
		text = m_level["name"].get<std::string>();
		if (text.size() > 15)
			textButton->setScale(.75f);
		break;
	case 101:
		textButton->setScale(.8f);
		text = m_level["author"].get<std::string>();
		break;
	case 102:
		textButton->setScale(.5f);
		text = m_level["id"].get<std::string>();
		break;
	}

	utils::setClipboardText(text.c_str());
	this->addChild(gd::TextAlertPopup::create("Copied to Clipboard", .5f, .6f));
}

void RouletteLayer::onPlayButton(CCObject*)
{
	if (m_list_fetcher.isFetching)
		return;

	nlohmann::json levelJson = m_level;

	auto level = gd::GJGameLevel::create();
	level->levelID_rand = std::stoi(levelJson["id"].get<std::string>()) + 5;
	level->levelID_seed = 5;
	level->levelID = std::stoi(levelJson["id"].get<std::string>());
	level->levelName = levelJson["name"].get<std::string>();
	level->userName = levelJson["author"].get<std::string>();
	level->accountID_rand = std::stoi(levelJson["accountID"].get<std::string>());
	level->accountID_seed = 0;
	level->accountID = std::stoi(levelJson["accountID"].get<std::string>());
	try
	{
		level->songID = levelJson["songID"].get<int>();
	}
	catch (...)
	{
		// song is not a custom one, returns "Level 1", remove "Level "
		level->audioTrack = std::stoi(levelJson["songID"].get<std::string>().substr(6));
	}
	level->levelNotDownloaded = true;
	level->levelType = gd::GJLevelType::kGJLevelTypeSaved;

	auto layer = gd::LevelInfoLayer::create(level);
	layer->downloadLevel();
	auto scene = CCScene::create();
	scene->addChild(layer);
	CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

void RouletteLayer::onNextButton(CCObject*)
{
	if (m_list_fetcher.isFetching)
		return;

	if (RouletteManager.lastLevelPercentage == 100)
	{
		onNextLevel(false, false);

		reinterpret_cast<CCLabelBMFont*>(m_pFinishedMenu->getChildByTag(2))->setString(
			CCString::createWithFormat("Skips Used: %d", RouletteManager.skipsUsed)->getCString()
		);
		reinterpret_cast<CCLabelBMFont*>(m_pFinishedMenu->getChildByTag(3))->setString(
			CCString::createWithFormat("Levels Played: %d", RouletteManager.numLevels)->getCString()
		);

		m_pPlayingMenu->setVisible(false);
		m_pFinishedMenu->setVisible(true);
	}
	else if (RouletteManager.lastLevelPercentage != 0 && RouletteManager.hasFinishedPreviousLevel)
	{
		RouletteManager.hasFinishedPreviousLevel = false;

		onNextLevel(false, true, -125.f);

		reinterpret_cast<CCLabelBMFont*>(
			m_pPlayingMenu->getChildByTag(20)
		)->setString(CCString::createWithFormat("%d%%", RouletteManager.lastLevelPercentage)->getCString());

		int diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
		int demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
		int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

		getRandomListLevel(difficulty, m_level, m_list_fetcher_response);
	}
	else
	{
		this->addChild(gd::TextAlertPopup::create(
			CCString::createWithFormat(
				"You need to get at least %d%%!", static_cast<int>(RouletteManager.levelPercentageGoal)
			)->getCString(), 1.2f, .8f
		));
	}
}

void RouletteLayer::onResetButton(CCObject*)
{
	if (m_list_fetcher.isFetching)
		return;

	RouletteManager.reset();

	m_level = {};

	// 
	/*for (int i = 0; i < 6; i++)
		m_pMainMenu->getChildByTag(i)->setPositionY(i < 3 ? 40.f : -25.f);*/

	onNextLevel();

	m_pMainMenu->setVisible(true);
	m_pPlayingMenu->setVisible(false);
	m_pFinishedMenu->setVisible(false);
	m_pErrorMenu->setVisible(false);

	reinterpret_cast<CCLabelBMFont*>(m_pPlayingMenu->getChildByTag(20))->setString("0%");

	m_pMainMenu->getChildByTag(11)->setPositionY(-20.f);
	m_pMainMenu->getChildByTag(12)->setVisible(true);
	m_pMainMenu->getChildByTag(12)->setPositionY(-85.f);

	if (RouletteManager.difficultyArr[5])
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

	if (RouletteManager.lastLevelPercentage == 100)
	{
		onNextButton(nullptr);
		return;
	}

	if (RouletteManager.skipsUsed < RouletteManager.maxSkips)
	{
		RouletteManager.skipsUsed++;
		RouletteManager.hasFinishedPreviousLevel = false;

		onNextLevel(false, true, -125.f);

		int diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
		int demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
		int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

		getRandomListLevel(difficulty, m_level, m_list_fetcher_response);
	}
	else
	{
		this->addChild(
			gd::TextAlertPopup::create(
				"You don't have any skips left!", 1.2f, .8f
			)
		);
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

	if (m_level.size() == 0)
	{
		reinterpret_cast<CCLabelBMFont*>(
			m_pErrorMenu->getChildByTag(2)
		)->setString(utils::vars::curlCodeToString[m_list_fetcher_response.responseCode]);
		
		m_pPlayingMenu->setVisible(false);
		m_pErrorMenu->setVisible(true);
		return;
	}

	onNextLevel(true);

	RouletteManager.lastLevelID = std::stoi(m_level["id"].get<std::string>());
	RouletteManager.levelCreatorName = m_level["author"].get<std::string>();

	if (m_level["name"].get<std::string>().size() > 15)
	{
		reinterpret_cast<CCLabelBMFont*>(
			m_pPlayingMenu->getChildByTag(1)->getChildren()->objectAtIndex(0)
		)->setScale(.75f);
	}
	reinterpret_cast<CCLabelBMFont*>(
		m_pPlayingMenu->getChildByTag(1)->getChildren()->objectAtIndex(0)
	)->setString(m_level["name"].get<std::string>().c_str());
	reinterpret_cast<CCLabelBMFont*>(
		m_pPlayingMenu->getChildByTag(2)->getChildren()->objectAtIndex(0)
	)->setString(("by " + m_level["author"].get<std::string>()).c_str());
	reinterpret_cast<CCLabelBMFont*>(
		m_pPlayingMenu->getChildByTag(3)->getChildren()->objectAtIndex(0)
	)->setString(("ID: " + m_level["id"].get<std::string>()).c_str());

	auto diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
	auto demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
	int difficultyTag = m_difficulty_to_tag.at(m_level["difficulty"].get<std::string>());

	if (int coins = m_level["coins"].get<int>(); coins > 0)
	{
		m_pPlayingMenu->getChildByTag(difficultyTag)->setPositionY(55.f);

		for (int i = 0; i < 3; i++)
			m_pPlayingMenu->getChildByTag(i + 15)->setPosition({ -120.f + 10.f * i, 13.f });

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

	if (m_level["epic"].get<bool>())
	{
		m_pPlayingMenu->getChildByTag(18)->setPosition(
			m_pPlayingMenu->getChildByTag(difficultyTag)->getPosition()
		);
		m_pPlayingMenu->getChildByTag(18)->setVisible(true);
	}

	if (m_level["featured"].get<bool>())
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


void RouletteLayer::getRandomListLevel(int difficulty, nlohmann::json& list, curlResponse& cfr)
{
	int listType = utils::getIndexOf(RouletteManager.selectedListArr, true);
	std::thread getListThread;

	switch (listType)
	{
	case 0:
		getListThread = std::thread([&](int difficulty, nlohmann::json& list, curlResponse& cfr) {
			m_list_fetcher.getRandomNormalListLevel(difficulty, std::ref(list), std::ref(cfr));
		}, difficulty, std::ref(list), std::ref(cfr));
		break;
	case 1:
		getListThread = std::thread([&](nlohmann::json& list, curlResponse& cfr) {
			m_list_fetcher.getRandomDemonListLevel(std::ref(list), std::ref(cfr));
		}, std::ref(list), std::ref(cfr));
		break;
	case 2:
		getListThread = std::thread([&](nlohmann::json& list, curlResponse& cfr) {
			m_list_fetcher.getRandomChallengeListLevel(std::ref(list), std::ref(cfr));
		}, std::ref(list), std::ref(cfr));
		break;
	default:
		break;
	}

	// manually set to true because sometimes the thread detatches too late
	m_list_fetcher.isFetching = true;
	this->scheduleUpdate();
	getListThread.detach();
}

gd::CCMenuItemSpriteExtra* RouletteLayer::createDifficultyButton(int tag, CCNode* sprite, CCPoint point, float scale, bool isDemon, bool visible)
{
	sprite->setScale(scale);
	auto button = gd::CCMenuItemSpriteExtra::create(
		sprite,
		this,
		menu_selector(RouletteLayer::onDifficultyChosen)
	);
	button->setPosition(point);
	button->setTag(tag);
	if (isDemon)
	{
		if (!RouletteManager.difficultyArr[tag - 6])
			button->setColor({ 125, 125, 125 });
	}
	else
	{
		if (!RouletteManager.difficultyArr[tag])
			button->setColor({ 125, 125, 125 });
	}
	button->setVisible(visible);
	m_pMainMenu->addChild(button);

	return button;
}
