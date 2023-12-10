#include <nlohmann/json.hpp>
#include <thread>
#include "RouletteLayer.hpp"
#include "RouletteInfoLayer.hpp"
#include "../../layers/IntegerInputLayer.hpp"
#include "../../utils.hpp"

#define INITIALIZEROULETTEMANAGER
#include "../manager/RouletteManager.hpp"


nlohmann::json RouletteLayer::level;
ListFetcher RouletteLayer::listFetcher;
curlResponse RouletteLayer::listFetcherResponse;

RouletteLayer* RouletteLayer::create()
{
	auto ret = new RouletteLayer();
	listFetcher.init();

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
		{ -120.f, -40.f }, 1.0f, true, false
	);
	createDifficultyButton(
		7, CCSprite::createWithSpriteFrameName("difficulty_08_btn2_001.png"),
		{ -60.f, -40.f }, 1.0f, true, false
	);
	createDifficultyButton(
		8, CCSprite::createWithSpriteFrameName("difficulty_06_btn2_001.png"),
		{ .0f, -40.f }, 1.f, true, false
	);
	createDifficultyButton(
		9, CCSprite::createWithSpriteFrameName("difficulty_09_btn2_001.png"),
		{ 60.f, -40.f }, 1.0f, true, false
	);
	createDifficultyButton(
		10, CCSprite::createWithSpriteFrameName("difficulty_10_btn2_001.png"),
		{ 120.f, -40.f }, 1.0f, true, false
	);
	auto plusButton = createButton(
		"GJ_plus2Btn_001.png", { 135.f, -20.f },
		menu_selector(RouletteLayer::onPlusButton), 11
	);
	plusButton->setVisible(RouletteManager.difficultyArr[5]);

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
	m_pButtonMenu->addChild(startButton);


	levelLoadingCircle = gd::LoadingCircle::create();
	levelLoadingCircle->setPosition({ -285.f, -170.f });
	levelLoadingCircle->setVisible(false);
	m_pButtonMenu->addChild(levelLoadingCircle);

	auto levelNameItem = gd::CCMenuItemSpriteExtra::create(
		CCLabelBMFont::create("LevelName", "bigFont.fnt"),
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelNameItem->setPosition({ 45.f, 75.f });
	levelNameItem->setTag(100);
	levelNameItem->setVisible(false);
	m_pButtonMenu->addChild(levelNameItem);

	auto levelAuthorItem = gd::CCMenuItemSpriteExtra::create(
		CCLabelBMFont::create("LevelAuthor", "bigFont.fnt"),
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelAuthorItem->setPosition({ 45.f, 45.f });
	levelAuthorItem->setTag(101);
	levelAuthorItem->setScale(.8f);
	levelAuthorItem->setVisible(false);
	m_pButtonMenu->addChild(levelAuthorItem);

	auto levelIDItem = gd::CCMenuItemSpriteExtra::create(
		CCLabelBMFont::create("LevelID", "bigFont.fnt"),
		this,
		menu_selector(RouletteLayer::onLevelInfo)
	);
	levelIDItem->setPosition({ 45.f, 17.f });
	levelIDItem->setScale(.5f);
	levelIDItem->setTag(102);
	levelIDItem->setVisible(false);
	m_pButtonMenu->addChild(levelIDItem);

	for (int i = 1; i < 6; i++)
	{
		auto levelDifficultySprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("difficulty_%02u_btn_001.png", i)->getCString());
		levelDifficultySprite->setPosition({ -110.f, static_cast<float>(i + 80) });
		levelDifficultySprite->setScale(1.5f);
		levelDifficultySprite->setTag(102 + i);
		levelDifficultySprite->setVisible(false);

		m_pButtonMenu->addChild(levelDifficultySprite);
	}

	for (int i = 6; i < 11; i++)
	{
		auto levelDifficultySprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("difficulty_%02u_btn2_001.png", i)->getCString());
		levelDifficultySprite->setPosition({ -110.f, static_cast<float>(i + 80) });
		levelDifficultySprite->setScale(1.5f);
		levelDifficultySprite->setVisible(false);
		if (i == 6)
			levelDifficultySprite->setTag(110);
		else if (i == 9)
			levelDifficultySprite->setTag(111);
		else if (i == 10)
			levelDifficultySprite->setTag(112);
		else
			levelDifficultySprite->setTag(101 + i);
		m_pButtonMenu->addChild(levelDifficultySprite);
	}

	auto levelPlayButtonItem = gd::CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png"),
		this,
		menu_selector(RouletteLayer::onPlayButton)
	);
	levelPlayButtonItem->setPosition({ -110.f, -53.f });
	levelPlayButtonItem->setVisible(false);
	levelPlayButtonItem->setTag(113);
	m_pButtonMenu->addChild(levelPlayButtonItem);

	for (int i = 1; i < 4; i++)
	{
		auto levelCoinSprite = CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");
		levelCoinSprite->setPosition({ -120.f + 10.f * (i - 1), 13.f });
		levelCoinSprite->setScale(1.5f);
		levelCoinSprite->setVisible(false);
		levelCoinSprite->setTag(113 + i);
		if (i == 1)
			levelCoinSprite->setZOrder(-1);
		m_pButtonMenu->addChild(levelCoinSprite);
	}


	auto percentageText = CCLabelBMFont::create(
		CCString::createWithFormat("%d%%", RouletteManager.lastLevelPercentage)->getCString(), "goldFont.fnt"
	);
	percentageText->setPosition({ 50.f, -20.f });
	percentageText->setScale(.8f);
	percentageText->setVisible(false);
	percentageText->setTag(117);
	m_pButtonMenu->addChild(percentageText);

	auto percentageBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
	percentageBg->setPosition({ 20.f, -35.f });
	percentageBg->setVisible(false);
	percentageBg->setContentSize({ 60.f, 30.f });
	percentageBg->setAnchorPoint({ .0f, .0f });
	percentageBg->setColor({ 0, 0, 0 });
	percentageBg->setOpacity(100);
	percentageBg->setTag(118);
	m_pButtonMenu->addChild(percentageBg, -1);

	auto skipButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Skip", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, 1.f),
		this,
		menu_selector(RouletteLayer::onSkipButton)
	);
	skipButton->setPosition({ -20.f, -70.f });
	skipButton->setVisible(false);
	skipButton->setTag(119);
	m_pButtonMenu->addChild(skipButton);

	auto nextButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Next", 50, true, "bigFont.fnt", "GJ_button_01.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onNextButton)
	);
	nextButton->setPosition({ 50.f, -70.f });
	nextButton->setVisible(false);
	nextButton->setTag(120);
	m_pButtonMenu->addChild(nextButton);

	auto resetButton = gd::CCMenuItemSpriteExtra::create(
		gd::ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RouletteLayer::onResetButton)
	);
	resetButton->setPosition({ 120.f, -70.f });
	resetButton->setVisible(false);
	resetButton->setTag(121);
	m_pButtonMenu->addChild(resetButton);


	auto ggText = CCLabelBMFont::create("GG!", "goldFont.fnt");
	ggText->setPosition({ .0f, 60.f });
	ggText->setVisible(false);
	ggText->setTag(122);
	m_pButtonMenu->addChild(ggText);

	auto ggSkipsUsedText = CCLabelBMFont::create("Skips Used: ", "bigFont.fnt");
	ggSkipsUsedText->setPosition({ .0f, 20.f });
	ggSkipsUsedText->setVisible(false);
	ggSkipsUsedText->setTag(123);
	m_pButtonMenu->addChild(ggSkipsUsedText);

	auto ggNumLevelsText = CCLabelBMFont::create("Levels Done: ", "bigFont.fnt");
	ggNumLevelsText->setPosition({ .0f, -10.f });
	ggNumLevelsText->setVisible(false);
	ggNumLevelsText->setTag(124);
	m_pButtonMenu->addChild(ggNumLevelsText);


	auto errorText = CCLabelBMFont::create("An error has occured", "bigFont.fnt");
	errorText->setPosition({ .0f, 10.f });
	errorText->setVisible(false);
	errorText->setColor({ 255, 0, 0 });
	errorText->setTag(125);
	m_pButtonMenu->addChild(errorText);

	auto errorReasonText = CCLabelBMFont::create("Error Reason", "bigFont.fnt");
	errorReasonText->setPosition({ .0f, -20.f });
	errorReasonText->setScale(.6f);
	errorReasonText->setVisible(false);
	errorReasonText->setTag(126);
	m_pButtonMenu->addChild(errorReasonText);


	auto optionsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsTxt_001.png");
	optionsSprite->setPosition({ 155.f, 110.f });
	optionsSprite->setVisible(RouletteManager.showOptionsSprite);
	optionsSprite->setTag(127);
	m_pButtonMenu->addChild(optionsSprite);


	if (RouletteManager.difficultyArr[5])
	{
		if (RouletteLayer::isPlusButtonToggled)
		{
			RouletteLayer::isPlusButtonToggled = false;
			RouletteLayer::onPlusButton(nullptr);
		}

		for (int i = 6; i < 11; i++)
			if (RouletteManager.demonDifficultyArr[i - 6])
				reinterpret_cast<gd::CCMenuItemSpriteExtra*>(
					m_pButtonMenu->getChildByTag(i)
				)->setColor({ 255, 255, 255 });
	}

	if (RouletteManager.lastLevelID != 0)
		RouletteManager.isPlayingRoulette = true;

	if (RouletteManager.isPlayingRoulette)
		onStartButton(nullptr);


	return true;
}

// called every frame after the ListFetcher thread has been detatched
void RouletteLayer::update(float dt)
{
	if (!listFetcher.isFetching)
	{
		finishLevelRoulette();
		this->unscheduleUpdate();
	}
}

void RouletteLayer::onClose(CCObject* sender)
{
	RouletteManager.isPlayingRoulette = false;

	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void RouletteLayer::onInfoButton(CCObject* sender)
{
	if (!RouletteManager.isPlayingRoulette)
	{
		if (RouletteManager.showOptionsSprite)
		{
			reinterpret_cast<gd::CCMenuItemSpriteExtra*>(sender)->getParent()->getChildByTag(127)->setVisible(false);
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

	ccColor3B color{};
	auto tag = sender->getTag();
	auto difficultyButton = reinterpret_cast<gd::CCMenuItemSpriteExtra*>(sender);
	auto& diffArr = RouletteManager.difficultyArr;
	auto& demonDiffArr = RouletteManager.demonDifficultyArr;

	// check if difficultyButton is one of the demon types and not a regular difficulty
	if (tag > 5 && (tag < 10 || tag > 5))
	{
		int ind = utils::getIndexOf(demonDiffArr, true);
		reinterpret_cast<gd::CCMenuItemSpriteExtra*>(
			m_pButtonMenu->getChildByTag(ind + 6)
		)->setColor({ 125, 125, 125 });

		demonDiffArr.at(ind) = false;
		demonDiffArr.at(tag - 6) = true;
	}
	else
	{
		if (isPlusButtonToggled && tag != 5)
			onPlusButton(nullptr);

		int ind = utils::getIndexOf(diffArr, true);
		reinterpret_cast<gd::CCMenuItemSpriteExtra*>(
			m_pButtonMenu->getChildByTag(ind)
		)->setColor({ 125, 125, 125 });

		diffArr.at(ind) = false;
		diffArr.at(tag) = true;
	}

	difficultyButton->setColor({ 255, 255, 255 });

	// demon
	m_pButtonMenu->getChildByTag(11)->setVisible(diffArr[5]);
}

void RouletteLayer::onStartButton(CCObject* sender)
{
	for (int i = 0; i < 13; i++)
		m_pButtonMenu->getChildByTag(i)->setVisible(false);

	if (RouletteManager.isPlayingRoulette)
	{
		finishLevelRoulette();
		return;
	}

	int diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
	int demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
	int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

	getRandomListLevel(difficulty, level, listFetcherResponse);

	levelLoadingCircle->setVisible(true);
	levelLoadingCircle->runAction(CCRepeatForever::create(cocos2d::CCRotateBy::create(1.f, 360)));

	RouletteManager.isPlayingRoulette = true;
}

void RouletteLayer::onPlusButton(CCObject* sender)
{
	isPlusButtonToggled = !isPlusButtonToggled;

	if (isPlusButtonToggled)
	{
		for (int i = 0; i < 6; i++)
			this->m_pButtonMenu->getChildByTag(i)->setPositionY(i < 3 ? 64.f : 10.f);

		this->m_pButtonMenu->getChildByTag(11)->setPositionY(15.f);
		this->m_pButtonMenu->getChildByTag(12)->setPositionY(-90.f);
	}
	else
	{
		for (int i = 0; i < 6; i++)
			this->m_pButtonMenu->getChildByTag(i)->setPositionY(i < 3 ? 40.f : -25.f);

		this->m_pButtonMenu->getChildByTag(11)->setPositionY(-20.f);
		this->m_pButtonMenu->getChildByTag(12)->setPositionY(-85.f);
	}

	for (int i = 6; i < 11; i++)
		this->m_pButtonMenu->getChildByTag(i)->setVisible(isPlusButtonToggled);
}

void RouletteLayer::onLevelInfo(CCObject* sender)
{
	auto buttonItem = reinterpret_cast<gd::CCMenuItemSpriteExtra*>(sender);
	std::string text;

	switch (buttonItem->getTag())
	{
	case 100:
		text = level["name"].get<std::string>();
		if (text.size() > 15)
			buttonItem->setScale(.75f);
		break;
	case 101:
		buttonItem->setScale(.8f);
		text = level["author"].get<std::string>();
		break;
	case 102:
		buttonItem->setScale(.5f);
		text = level["id"].get<std::string>();
		break;
	}

	utils::setClipboardText(text.c_str());
	this->addChild(gd::TextAlertPopup::create("Copied to Clipboard", .5f, .6f));
}

void RouletteLayer::onPlayButton(CCObject* sender)
{
	if (listFetcher.isFetching)
		return;

	nlohmann::json levelJson = level;

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

void RouletteLayer::onNextButton(CCObject* sender)
{
	if (listFetcher.isFetching)
		return;

	if (RouletteManager.lastLevelPercentage == 100)
	{
		for (int i = 0; i < 13; i++)
			m_pButtonMenu->getChildByTag(100 + i)->setVisible(false);

		for (int i = 0; i < 8; i++)
			m_pButtonMenu->getChildByTag(113 + i)->setVisible(false);

		reinterpret_cast<gd::CCMenuItemSpriteExtra*>(m_pButtonMenu->getChildByTag(121))->setPosition({ .0f, -70.f });
		m_pButtonMenu->getChildByTag(122)->setVisible(true);
		reinterpret_cast<CCLabelBMFont*>(m_pButtonMenu->getChildByTag(123))->setString(
			CCString::createWithFormat("Skips Used: %d", RouletteManager.skipsCount)->getCString()
		);
		m_pButtonMenu->getChildByTag(123)->setVisible(true);
		reinterpret_cast<CCLabelBMFont*>(m_pButtonMenu->getChildByTag(124))->setString(
			CCString::createWithFormat("Levels Done: %d", RouletteManager.numLevels)->getCString()
		);
		m_pButtonMenu->getChildByTag(124)->setVisible(true);
	}
	else if (RouletteManager.lastLevelPercentage != 0 && RouletteManager.hasFinishedPreviousLevel)
	{
		RouletteManager.hasFinishedPreviousLevel = false;

		levelLoadingCircle->setPositionY(-125.f);
		levelLoadingCircle->setVisible(true);
		levelLoadingCircle->runAction(CCRepeatForever::create(cocos2d::CCRotateBy::create(1.f, 360)));

		for (int i = 0; i < 13; i++)
			m_pButtonMenu->getChildByTag(100 + i)->setVisible(false);

		for (int i = 0; i < 3; i++)
			m_pButtonMenu->getChildByTag(114 + i)->setVisible(false);

		reinterpret_cast<CCLabelBMFont*>(
			m_pButtonMenu->getChildByTag(117)
		)->setString(CCString::createWithFormat("%d%%", RouletteManager.lastLevelPercentage)->getCString());

		int diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
		int demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
		int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

		getRandomListLevel(difficulty, level, listFetcherResponse);
	}
	else
	{
		this->addChild(gd::TextAlertPopup::create(
			CCString::createWithFormat(
				"You need to get at least %d%%", static_cast<int>(RouletteManager.levelPercentageGoal)
			)->getCString(), 1.2f, .8f
		));

		return;
	}

	if (levelEpicSprite && levelEpicSprite->getParent())
		levelEpicSprite->removeFromParentAndCleanup(true);

	if (levelFeaturedSprite && levelFeaturedSprite->getParent())
		levelFeaturedSprite->removeFromParentAndCleanup(true);
}

void RouletteLayer::onResetButton(CCObject* sender)
{
	if (listFetcher.isFetching)
		return;

	RouletteManager.isPlayingRoulette = false;
	RouletteManager.hasFinishedPreviousLevel = false;
	RouletteManager.lastLevelID = 0;
	RouletteManager.lastLevelPercentage = 0;
	RouletteManager.levelPercentageGoal = 1;
	RouletteManager.skipsCount = 0;

	if (levelEpicSprite && levelEpicSprite->getParent())
		levelEpicSprite->removeFromParentAndCleanup(true);

	if (levelFeaturedSprite && levelFeaturedSprite->getParent())
		levelFeaturedSprite->removeFromParentAndCleanup(true);

	level = {};

	for (int i = 0; i < 6; i++)
	{
		m_pButtonMenu->getChildByTag(i)->setPositionY(i < 3 ? 40.f : -25.f);
		m_pButtonMenu->getChildByTag(i)->setVisible(true);
	}

	for (int i = 0; i < 6; i++)
		m_pButtonMenu->getChildByTag(6 + i)->setVisible(false);

	for (int i = 0; i < 27; i++)
		m_pButtonMenu->getChildByTag(100 + i)->setVisible(false);

	reinterpret_cast<CCLabelBMFont*>(m_pButtonMenu->getChildByTag(117))->setString("0%");
	m_pButtonMenu->getChildByTag(121)->setPosition({ 120.f, -70.f });

	m_pButtonMenu->getChildByTag(11)->setPositionY(-20.f);
	m_pButtonMenu->getChildByTag(12)->setVisible(true);
	m_pButtonMenu->getChildByTag(12)->setPositionY(-85.f);

	if (RouletteManager.difficultyArr[5])
	{
		isPlusButtonToggled = false;
		m_pButtonMenu->getChildByTag(11)->setVisible(true);
	}

	levelLoadingCircle->setPositionY(-170.f);
}

void RouletteLayer::onSkipButton(CCObject* sender)
{
	if (listFetcher.isFetching)
		return;

	if (RouletteManager.levelPercentageGoal == 101)
	{
		onNextButton(nullptr);
		return;
	}


	if (RouletteManager.skipsCount < RouletteManager.maxSkips)
	{
		if (levelEpicSprite && levelEpicSprite->getParent())
			levelEpicSprite->removeFromParentAndCleanup(true);

		if (levelFeaturedSprite && levelFeaturedSprite->getParent())
			levelFeaturedSprite->removeFromParentAndCleanup(true);

		RouletteManager.skipsCount++;
		RouletteManager.hasFinishedPreviousLevel = false;

		levelLoadingCircle->setPositionY(-125.f);
		levelLoadingCircle->setVisible(true);
		levelLoadingCircle->runAction(CCRepeatForever::create(cocos2d::CCRotateBy::create(1.f, 360)));

		for (int i = 0; i < 13; i++)
			m_pButtonMenu->getChildByTag(100 + i)->setVisible(false);

		for (int i = 0; i < 3; i++)
			m_pButtonMenu->getChildByTag(114 + i)->setVisible(false);

		int diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
		int demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
		int difficulty = diffInd == 5 ? (demonInd + 6) : (diffInd + 1);

		getRandomListLevel(difficulty, level, listFetcherResponse);
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

void RouletteLayer::finishLevelRoulette()
{
	levelLoadingCircle->stopAllActions();
	levelLoadingCircle->setVisible(false);

	if (level.size() == 0)
	{
		for (int i = 0; i < 11; i++)
			m_pButtonMenu->getChildByTag(113 + i)->setVisible(false);

		m_pButtonMenu->getChildByTag(121)->setVisible(true);
		m_pButtonMenu->getChildByTag(125)->setVisible(true);
		m_pButtonMenu->getChildByTag(126)->setVisible(true);
		reinterpret_cast<CCLabelBMFont*>(
			m_pButtonMenu->getChildByTag(126)
		)->setString(utils::vars::curlCodeToString[listFetcherResponse.responseCode]);
		return;
	}

	RouletteManager.lastLevelID = std::stoi(level["id"].get<std::string>());
	RouletteManager.levelCreatorName = level["author"].get<std::string>();

	if (level["name"].get<std::string>().size() > 15)
	{
		reinterpret_cast<CCLabelBMFont*>(
			m_pButtonMenu->getChildByTag(100)->getChildren()->objectAtIndex(0)
		)->setScale(.75f);
	}
	reinterpret_cast<CCLabelBMFont*>(
		m_pButtonMenu->getChildByTag(100)->getChildren()->objectAtIndex(0)
	)->setString(level["name"].get<std::string>().c_str());
	reinterpret_cast<CCLabelBMFont*>(
		m_pButtonMenu->getChildByTag(101)->getChildren()->objectAtIndex(0)
	)->setString(("by " + level["author"].get<std::string>()).c_str());
	reinterpret_cast<CCLabelBMFont*>(
		m_pButtonMenu->getChildByTag(102)->getChildren()->objectAtIndex(0)
	)->setString(("ID: " + level["id"].get<std::string>()).c_str());

	for (int i = 0; i < 3; i++)
		m_pButtonMenu->getChildByTag(100 + i)->setVisible(true);

	m_pButtonMenu->getChildByTag(113)->setVisible(true);

	for (int i = 0; i < 5; i++)
		m_pButtonMenu->getChildByTag(117 + i)->setVisible(true);

	auto diffInd = utils::getIndexOf(RouletteManager.difficultyArr, true);
	auto demonInd = utils::getIndexOf(RouletteManager.demonDifficultyArr, true);
	int difficultyTag = difficultyToTag[level["difficulty"].get<std::string>()];

	if (int coins = level["coins"].get<int>(); coins > 0)
	{
		m_pButtonMenu->getChildByTag(difficultyTag)->setPositionY(55.f);

		for (int i = 0; i < 3; i++)
			m_pButtonMenu->getChildByTag(113 + i + 1)->setPosition({ -120.f + 10.f * i, 13.f });

		switch (coins)
		{
		case 1:
			m_pButtonMenu->getChildByTag(115)->setVisible(true);
			if (diffInd == 5)
				m_pButtonMenu->getChildByTag(115)->setPositionY(.0f);
			break;
		case 2:
			m_pButtonMenu->getChildByTag(114)->setVisible(true);
			m_pButtonMenu->getChildByTag(115)->setVisible(true);

			m_pButtonMenu->getChildByTag(114)->setPositionX(-115.f);
			m_pButtonMenu->getChildByTag(115)->setPositionX(-105.f);

			if (diffInd == 5)
			{
				m_pButtonMenu->getChildByTag(114)->setPositionY(.0f);
				m_pButtonMenu->getChildByTag(115)->setPositionY(.0f);
			}
			break;
		case 3:
			for (int i = 0; i < coins; i++)
			{
				m_pButtonMenu->getChildByTag(113 + i + 1)->setVisible(true);
				if (level["stars"].get<int>() > 5)
					m_pButtonMenu->getChildByTag(113 + i + 1)->setPositionY(.0f);
			}
			break;
		}
	}
	else
		m_pButtonMenu->getChildByTag(difficultyTag)->setPositionY(40.f);

	m_pButtonMenu->getChildByTag(difficultyTag)->setVisible(true);

	if (level["epic"].get<bool>())
	{
		auto difficultySprite = m_pButtonMenu->getChildByTag(difficultyTag);
		levelEpicSprite = CCSprite::createWithSpriteFrameName("GJ_epicCoin_001.png");
		levelEpicSprite->setScale(1.4f);
		levelEpicSprite->setPosition(difficultySprite->getPosition());
		m_pButtonMenu->addChild(levelEpicSprite, -1);
	}

	if (level["featured"].get<bool>())
	{
		auto difficultySprite = m_pButtonMenu->getChildByTag(difficultyTag);
		levelFeaturedSprite = CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");
		levelFeaturedSprite->setScale(1.5f);
		levelFeaturedSprite->setPosition(difficultySprite->getPosition());
		m_pButtonMenu->addChild(levelFeaturedSprite, -1);
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
			listFetcher.getRandomNormalListLevel(difficulty, std::ref(list), std::ref(cfr));
		}, difficulty, std::ref(list), std::ref(cfr));
		break;
	case 1:
		getListThread = std::thread([&](nlohmann::json& list, curlResponse& cfr) {
			listFetcher.getRandomDemonListLevel(std::ref(list), std::ref(cfr));
		}, std::ref(list), std::ref(cfr));
		break;
	case 2:
		getListThread = std::thread([&](nlohmann::json& list, curlResponse& cfr) {
			listFetcher.getRandomChallengeListLevel(std::ref(list), std::ref(cfr));
		}, std::ref(list), std::ref(cfr));
		break;
	default:
		break;
	}

	// manually set to true because sometimes the thread detatches too late
	listFetcher.isFetching = true;
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
	m_pButtonMenu->addChild(button);

	return button;
}
