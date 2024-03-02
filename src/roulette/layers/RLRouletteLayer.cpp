#include <thread>
#include <matjson.hpp>
#include <matjson/stl_serialize.hpp>
#include <rtrp/objects/LevelObject.hpp>
#include "RLRouletteLayer.hpp"
#include "RLRouletteInfoLayer.hpp"
#include "../manager/RouletteManager.hpp"
#include "../../custom_nodes/RLDifficultyNode.hpp"
#include "../../utils.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;


ListFetcher::level_pair_t RLRouletteLayer::m_level;
ListFetcher RLRouletteLayer::m_list_fetcher;
std::string RLRouletteLayer::m_list_fetcher_error;

RLRouletteLayer* RLRouletteLayer::create()
{
	auto ret = new RLRouletteLayer();
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

bool RLRouletteLayer::init()
{
	if (!this->createBasics({ 440.f, 290.f }, menu_selector(RLRouletteLayer::onClose))) return false;

	m_selected_difficulty = g_rouletteManager.previousDifficulty;
	m_selected_demon_difficulty = g_rouletteManager.previousDemonDifficulty;

	this->setID("RLRouletteLayer");

	main_menu = CCMenu::create();
	playing_menu = CCMenu::create();
	finished_menu = CCMenu::create();
	error_menu = CCMenu::create();

	m_buttonMenu->setID("button-menu");
	main_menu->setID("main-menu");

	playing_menu->setID("playing-menu");
	playing_menu->setVisible(false);

	finished_menu->setID("finished-menu");
	finished_menu->setVisible(false);

	error_menu->setID("error-menu");
	error_menu->setVisible(false);

	m_mainLayer->addChild(main_menu, 11);
	m_mainLayer->addChild(playing_menu, 11);
	m_mainLayer->addChild(finished_menu, 11);
	m_mainLayer->addChild(error_menu, 11);


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

	createButton("GJ_infoIcon_001.png", { 200.f, 125.f }, menu_selector(RLRouletteLayer::onInfoButton));
	auto optionsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsTxt_001.png");
	optionsSprite->setPosition({ 155.f, 110.f });
	optionsSprite->setVisible(!Mod::get()->setSavedValue<bool>("show-options-sprite", true));
	optionsSprite->setID("options-sprite");
	m_buttonMenu->addChild(optionsSprite);

	createDifficultyButton(
		GJDifficulty::Easy,
		CCSprite::createWithSpriteFrameName("difficulty_01_btn_001.png"),
		{ -90.f, 40.f }, 1.2f
	);
	createDifficultyButton(
		GJDifficulty::Normal,
		CCSprite::createWithSpriteFrameName("difficulty_02_btn_001.png"),
		{ .0f, 40.f }, 1.2f
	);
	createDifficultyButton(
		GJDifficulty::Hard,
		CCSprite::createWithSpriteFrameName("difficulty_03_btn_001.png"),
		{ 90.f, 40.f }, 1.2f
	);
	createDifficultyButton(
		GJDifficulty::Harder,
		CCSprite::createWithSpriteFrameName("difficulty_04_btn_001.png"),
		{ -90.f, -25.f }, 1.2f
	);
	createDifficultyButton(
		GJDifficulty::Insane,
		CCSprite::createWithSpriteFrameName("difficulty_05_btn_001.png"),
		{ .0f, -25.f }, 1.2f
	);
	createDifficultyButton(
		GJDifficulty::Demon,
		RLDifficultyNode::create(
			m_selected_difficulty >= GJDifficulty::Demon ? m_selected_demon_difficulty : static_cast<GJDifficulty>(-2),
			false, false
		),
		{ 90.f, -25.f }, 1.2f
	);
	auto plusButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
		this,
		menu_selector(RLRouletteLayer::onPlusButton)
	);
	plusButton->setPosition({ 135.f, -20.f });
	plusButton->setSizeMult(1.2f);
	plusButton->setID("demon-plus-button");
	plusButton->setVisible(m_selected_difficulty >= GJDifficulty::Demon);
	main_menu->addChild(plusButton);

	auto startButtonText = CCLabelBMFont::create("Start", "bigFont.fnt");
	startButtonText->setPosition({ 46.f, 18.f });
	startButtonText->setScale(.725f);
	auto startButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_longBtn02_001.png"),
		this,
		menu_selector(RLRouletteLayer::onStartButton)
	);
	startButton->setPosition({ .0f, -85.f });
	startButton->setID("start-button");
	startButton->addChild(startButtonText);
	main_menu->addChild(startButton);


	m_loading_circle = LoadingCircle::create();
	m_loading_circle->setPosition({ -285.f, -170.f });
	m_loading_circle->setVisible(false);
	m_buttonMenu->addChild(m_loading_circle);


	auto levelNameButtonLabel = CCLabelBMFont::create("LevelName", "bigFont.fnt");
	levelNameButtonLabel->limitLabelWidth(120.f, 1.f, .1f);
	levelNameButtonLabel->setID("button-label");
	auto levelNameButton = CCMenuItemSpriteExtra::create(
		levelNameButtonLabel,
		this,
		menu_selector(RLRouletteLayer::onLevelInfo)
	);
	levelNameButton->setSizeMult(1.2f);
	levelNameButton->setPosition({ 45.f, 75.f });
	levelNameButton->setID("level-name-button");
	levelNameButton->setTag(1);
	playing_menu->addChild(levelNameButton);

	auto levelCreatorButtonLabel = CCLabelBMFont::create("LevelCreator", "goldFont.fnt");
	levelCreatorButtonLabel->limitLabelWidth(140.f, .8f, .2f);
	levelCreatorButtonLabel->setID("button-label");
	auto levelCreatorButton = CCMenuItemSpriteExtra::create(
		levelCreatorButtonLabel,
		this,
		menu_selector(RLRouletteLayer::onLevelInfo)
	);
	levelCreatorButton->setSizeMult(1.2f);
	levelCreatorButton->setPosition({ 45.f, 45.f });
	levelCreatorButton->setID("level-creator-button");
	levelCreatorButton->setTag(2);
	playing_menu->addChild(levelCreatorButton);

	auto levelIDButtonLabel = CCLabelBMFont::create("LevelID", "goldFont.fnt");
	levelIDButtonLabel->setScale(.5f);
	levelIDButtonLabel->setID("button-label");
	auto levelIDButton = CCMenuItemSpriteExtra::create(
		levelIDButtonLabel,
		this,
		menu_selector(RLRouletteLayer::onLevelInfo)
	);
	levelIDButton->setSizeMult(1.2f);
	levelIDButton->setPosition({ 45.f, 17.f });
	levelIDButton->setID("level-id-button");
	levelIDButton->setTag(3);
	playing_menu->addChild(levelIDButton);

	auto levelPlayButtonItem = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png"),
		this,
		menu_selector(RLRouletteLayer::onPlayButton)
	);
	levelPlayButtonItem->setPosition({ -110.f, -53.f });
	levelPlayButtonItem->setID("play-button");
	levelPlayButtonItem->setTag(4);
	playing_menu->addChild(levelPlayButtonItem);

	auto difficultyNode = RLDifficultyNode::create(GJDifficulty::Normal, false, true);
	difficultyNode->setPosition({ -110.f, 40.f });
	difficultyNode->setScale(1.5f);
	difficultyNode->setID("difficulty-node");
	difficultyNode->setTag(5);
	playing_menu->addChild(difficultyNode);

	for (int i = 6; i < 9; i++)
	{
		auto levelCoinSprite = CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");
		levelCoinSprite->setPosition({ -120.f + 10.f * (i - 6), 13.f });
		levelCoinSprite->setScale(1.5f);
		levelCoinSprite->setVisible(false);
		levelCoinSprite->setID(fmt::format("coin-{}", i - 5));
		levelCoinSprite->setTag(i);
		if (i == 6)
			levelCoinSprite->setZOrder(-1);
		playing_menu->addChild(levelCoinSprite);
	}

	auto percentageText = CCLabelBMFont::create(
		fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str(),
		"goldFont.fnt"
	);
	percentageText->setPosition({ 50.f, -20.f });
	percentageText->setScale(.8f);
	percentageText->setID("percentage-text");
	playing_menu->addChild(percentageText);

	auto percentageBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
	percentageBg->setPosition({ 20.f, -35.f });
	percentageBg->setContentSize({ 60.f, 30.f });
	percentageBg->setAnchorPoint({ .0f, .0f });
	percentageBg->setColor({ 0, 0, 0 });
	percentageBg->setOpacity(100);
	percentageBg->setID("percentage-bg");
	playing_menu->addChild(percentageBg, -1);

	auto skipButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Skip", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, 1.f),
		this,
		menu_selector(RLRouletteLayer::onSkipButton)
	);
	skipButton->setPosition({ -20.f, -70.f });
	skipButton->setID("skip-button");
	playing_menu->addChild(skipButton);

	auto nextButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Next", 50, true, "bigFont.fnt", "GJ_button_01.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onNextButton)
	);
	nextButton->setPosition({ 50.f, -70.f });
	nextButton->setID("next-button");
	playing_menu->addChild(nextButton);

	auto resetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onResetButton)
	);
	resetButton->setPosition({ 120.f, -70.f });
	resetButton->setID("reset-button");
	playing_menu->addChild(resetButton);


	auto ggText = CCLabelBMFont::create("GG!", "goldFont.fnt");
	ggText->setPosition({ .0f, 60.f });
	ggText->setID("gg-label");
	finished_menu->addChild(ggText);

	auto ggSkipsUsedText = CCLabelBMFont::create("Skips Used: ", "bigFont.fnt");
	ggSkipsUsedText->setPosition({ .0f, 20.f });
	ggSkipsUsedText->setID("skips-label");
	finished_menu->addChild(ggSkipsUsedText);

	auto ggNumLevelsText = CCLabelBMFont::create("Levels Played: ", "bigFont.fnt");
	ggNumLevelsText->setPosition({ .0f, -10.f });
	ggNumLevelsText->setID("levels-played-label");
	finished_menu->addChild(ggNumLevelsText);

	auto finishedResetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onResetButton)
	);
	finishedResetButton->setPosition({ .0f, -70.f });
	finishedResetButton->setTag(4);
	finished_menu->addChild(finishedResetButton);


	auto errorText = CCLabelBMFont::create("An error has occurred", "bigFont.fnt");
	errorText->setPosition({ .0f, 10.f });
	errorText->setColor({ 255, 0, 0 });
	errorText->setScale(.8f);
	errorText->setID("title-label");
	error_menu->addChild(errorText);

	auto errorReasonText = TextArea::create(
		"ErrorReason",
		"chatFont.fnt",
		.85f, 290.f, { .5f, .5f }, 20.f, false
	);
	errorReasonText->setPosition({ 25.f, -20.f });
	errorReasonText->setID("reason-label");
	error_menu->addChild(errorReasonText);

	auto errorResetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onResetButton)
	);
	errorResetButton->setPosition({ 120.f, -70.f });
	errorResetButton->setID("reset-button");
	error_menu->addChild(errorResetButton);


	if (g_rouletteManager.isPaused)
	{
		g_rouletteManager.isPlaying = true;
		g_rouletteManager.isPaused = false;
		finishLevelRoulette();
	}

	return true;
}

// called every frame after the ListFetcher thread has been detached
void RLRouletteLayer::update(float)
{
	if (!m_list_fetcher.is_fetching)
	{
		finishLevelRoulette();
		this->unscheduleUpdate();
	}
}

void RLRouletteLayer::onClose(CCObject*)
{
	if (g_rouletteManager.isPlaying)
	{
		m_confirmation_layer = RLConfirmationAlertLayer::create({
			"Woah there!",
			"Would you like to <cr>quit</c> or <co>pause</c> the roulette?",
			[&](auto cl) {
				this->setKeypadEnabled(false);
				this->removeFromParentAndCleanup(true);
				g_rouletteManager.isPlaying = false;
				g_rouletteManager.isPaused = true;
			},
			[&](auto cl) {
				g_rouletteManager.reset();

				this->setKeypadEnabled(false);
				this->removeFromParentAndCleanup(true);
				g_rouletteManager.rouletteLayer = nullptr;
			},
			"Pause", "Quit"
		});
		if (m_confirmation_layer)
			m_confirmation_layer->show();
	}
	else
	{
		this->setKeypadEnabled(false);
		this->removeFromParentAndCleanup(true);
		g_rouletteManager.rouletteLayer = nullptr;
	}
}

void RLRouletteLayer::onInfoButton(CCObject*)
{
	if (!g_rouletteManager.isPlaying)
	{
		if (Mod::get()->getSavedValue<bool>("show-options-sprite"))
			m_buttonMenu->getChildByID("options-sprite")->setVisible(false);

		m_roulette_info_layer = RLRouletteInfoLayer::create();
		if (m_roulette_info_layer)
			m_roulette_info_layer->show();
	}
	else
		// this->addChild(TextAlertPopup::create("You are currently in a game of roulette!", 1.2f, .8f, 1, "bigFont.fnt"));
		Notification::create("You are currently in a game of roulette!", NotificationIcon::Error, .7f)->show();
}

void RLRouletteLayer::onDifficultyButton(CCObject* sender)
{
	if (rl::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("selected-list-array").as_array(), true) != 0)
		return;

	auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
	auto difficulty = static_cast<GJDifficulty>(sender->getTag());
	auto& array = g_rouletteManager.getFromSaveContainer("difficulty-array").as_array();
	int idx = rl::utils::getIndexOf(array, true);

	array.at(idx) = false;
	array.at(rl::constants::diff_to_idx.at(difficulty)) = true;
	g_rouletteManager.previousDifficulty = difficulty;
	m_selected_difficulty = difficulty;

	setDifficultyButtonColor(rl::constants::idx_to_diff.at(idx), { 125, 125, 125 });
	setDifficultyButtonColor(difficulty, { 255, 255, 255 });

	main_menu->getChildByID("demon-plus-button")->setVisible(m_selected_difficulty >= GJDifficulty::Demon);

	static_cast<RLDifficultyNode*>(
		g_rouletteManager.rouletteLayer->getDifficultyButton(GJDifficulty::Demon)->getNormalImage()
	)->setDifficulty(m_selected_difficulty == GJDifficulty::Demon ? g_rouletteManager.previousDemonDifficulty : static_cast<GJDifficulty>(-2));
}

void RLRouletteLayer::onStartButton(CCObject*)
{
	main_menu->setVisible(false);

	getRandomListLevel(
		m_selected_difficulty == GJDifficulty::Demon ? m_selected_demon_difficulty : m_selected_difficulty,
		m_level,
		m_list_fetcher_error
	);

	onNextLevel(false, true);

	g_rouletteManager.isPlaying = true;
}

void RLRouletteLayer::onPlusButton(CCObject*)
{
	m_demon_select_layer = RLDemonSelectLayer::create({
		m_selected_demon_difficulty,
		[&](GJDifficulty currentDifficulty, GJDifficulty previousDifficulty)
		{
			auto& array = g_rouletteManager.getFromSaveContainer("demon-difficulty-array").as_array();

			m_selected_demon_difficulty = currentDifficulty;

			array.at(rl::constants::demon_diff_to_idx.at(previousDifficulty)) = false;
			array.at(rl::constants::demon_diff_to_idx.at(currentDifficulty)) = true;
			g_rouletteManager.previousDemonDifficulty = currentDifficulty;

			static_cast<RLDifficultyNode*>(
				g_rouletteManager.rouletteLayer->getDifficultyButton(GJDifficulty::Demon)->getChildByID("sprite-node")
			)->setDifficulty(currentDifficulty);
		}
	});
	if (m_demon_select_layer)
		m_demon_select_layer->show();
}

void RLRouletteLayer::onLevelInfo(CCObject* sender)
{
	auto textButton = static_cast<CCMenuItemSpriteExtra*>(sender);
	std::string text;

	switch (textButton->getTag())
	{
	case 1: text = m_level.first.name; break;
	case 2: text = m_level.second.name; break;
	case 3: text = std::to_string(m_level.first.levelID); break;
	default: text = "[invalid]"; break;
	}

	clipboard::write(text);
	// this->addChild(TextAlertPopup::create("Copied to Clipboard", .5f, .6f, 1, "bigFont.fnt"));
	Notification::create("Copied to Clipboard", NotificationIcon::Success)->show();
}

void RLRouletteLayer::onPlayButton(CCObject*)
{
	if (m_list_fetcher.is_fetching)
		return;

	LevelInfoLayer* layer;
	if (m_level.first.levelID != 0)
	{
		layer = LevelInfoLayer::create(rl::utils::createLevelFromResponse(m_level), false);
		layer->downloadLevel();
	}
	else
		layer = LevelInfoLayer::create(GJGameLevel::create(), true);

	auto scene = CCScene::create();
	scene->addChild(layer);

	CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(.5f, scene));
}

void RLRouletteLayer::onNextButton(CCObject*)
{
	if (m_list_fetcher.is_fetching)
		return;

	if (g_rouletteManager.currentLevelPercentage == 100)
	{
		onNextLevel(false, false);

		static_cast<CCLabelBMFont*>(finished_menu->getChildByID("skips-label"))->setString(
			fmt::format("Skips Used: {}", g_rouletteManager.skipsUsed).c_str()
		);
		static_cast<CCLabelBMFont*>(finished_menu->getChildByID("levels-played-label"))->setString(
			fmt::format("Levels Played: {}", g_rouletteManager.numLevels).c_str()
		);

		playing_menu->setVisible(false);
		finished_menu->setVisible(true);
	}
	else if (g_rouletteManager.currentLevelPercentage != 0 && g_rouletteManager.hasFinishedPreviousLevel)
	{
		g_rouletteManager.hasFinishedPreviousLevel = false;

		onNextLevel(false, true, -125.f);

		static_cast<CCLabelBMFont*>(
			playing_menu->getChildByID("percentage-text")
		)->setString(fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str());

		getRandomListLevel(
			m_selected_difficulty == GJDifficulty::Demon ? m_selected_demon_difficulty : m_selected_difficulty,
			m_level,
			m_list_fetcher_error
		);
	}
	else
		// this->addChild(TextAlertPopup::create(
		// 	std::format(
		// 		"You need to get at least {}%!", static_cast<int>(g_rouletteManager.levelPercentageGoal)
		// 	), 1.2f, .8f, 1, "bigFont.fnt"
		// ));
		Notification::create(
			fmt::format("You need to get at least {}%!", g_rouletteManager.levelPercentageGoal),
			NotificationIcon::Error,
			.7f
		)->show();
}

void RLRouletteLayer::onResetButton(CCObject*)
{
	if (m_list_fetcher.is_fetching)
		return;

	g_rouletteManager.reset();

	m_level = { {}, {} };

	onNextLevel();

	main_menu->setVisible(true);
	playing_menu->setVisible(false);
	finished_menu->setVisible(false);
	error_menu->setVisible(false);

	static_cast<CCLabelBMFont*>(playing_menu->getChildByID("percentage-text"))->setString(
		fmt::format("{}%", g_rouletteManager.levelPercentageGoal).c_str()
	);

	main_menu->getChildByID("demon-plus-button")->setPositionY(-20.f);
	main_menu->getChildByID("start-button")->setVisible(true);
	main_menu->getChildByID("start-button")->setPositionY(-85.f);

	m_loading_circle->setPositionY(-170.f);
}

void RLRouletteLayer::onSkipButton(CCObject*)
{
	if (m_list_fetcher.is_fetching)
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

		getRandomListLevel(
			m_selected_difficulty == GJDifficulty::Demon ? m_selected_demon_difficulty : m_selected_difficulty,
			m_level,
			m_list_fetcher_error
		);
	}
	else
		// this->addChild(
		// 	TextAlertPopup::create(
		// 		"You don't have any skips left!", 1.2f, .8f, 1, "bigFont.fnt"
		// 	)
		// );
		Notification::create("You don't have any skips left!", NotificationIcon::Error, .7f)->show();
}

void RLRouletteLayer::finishLevelRoulette()
{
	m_loading_circle->stopAllActions();
	m_loading_circle->setVisible(false);

	if (!m_list_fetcher_error.empty())
	{
		static_cast<TextArea*>(error_menu->getChildByID("reason-label"))->setString(m_list_fetcher_error);

		playing_menu->setVisible(false);
		error_menu->setVisible(true);
		g_rouletteManager.isPlaying = false;
		return;
	}

	onNextLevel(true);
	main_menu->setVisible(false);

	g_rouletteManager.currentLevelID = m_level.first.levelID;

	static_cast<CCLabelBMFont*>(
		playing_menu->getChildByID("level-name-button")->getChildByID("button-label")
	)->setString(m_level.first.name.c_str());
	static_cast<CCLabelBMFont*>(
		playing_menu->getChildByID("level-creator-button")->getChildByID("button-label")
	)->setString(fmt::format("by {}", m_level.second.name).c_str());
	static_cast<CCLabelBMFont*>(
		playing_menu->getChildByID("level-id-button")->getChildByID("button-label")
	)->setString(fmt::format("ID: {}", m_level.first.levelID).c_str());

	static_cast<RLDifficultyNode*>(playing_menu->getChildByID("difficulty-node"))->setDifficulty(
		rl::utils::getDifficultyFromResponse(m_level.first), m_level.first.extras.featured, m_level.first.extras.epic
	);
	playing_menu->getChildByID("difficulty-node")->setVisible(true);

	if (int coins = m_level.first.coins; coins > 0)
	{
		auto difficulty = rl::utils::getDifficultyFromResponse(m_level.first);
		if (m_level.first.extras.featured || m_level.first.extras.epic)
			playing_menu->getChildByID("difficulty-node")->setPositionY(50.f);
		else
			playing_menu->getChildByID("difficulty-node")->setPositionY(55.f);
		const float difficultyNodeYPos = playing_menu->getChildByID("difficulty-node")->getPositionY();
		const float coinsYOffset = difficulty >= GJDifficulty::Demon ? 56.f : 45.f;

		for (int i = 6; i < 9; i++)
		{
			float x_pos = rl::constants::coins_x_pos[coins - 1][i - 6];

			if (x_pos != .0f)
				playing_menu->getChildByTag(i)->setPositionX(x_pos);
			playing_menu->getChildByTag(i)->setPositionY(difficultyNodeYPos - coinsYOffset);
			playing_menu->getChildByTag(i)->setVisible(x_pos != .0f);
		}
	}
	else
		playing_menu->getChildByID("difficulty-node")->setPositionY(40.f);

	playing_menu->setVisible(true);
}

void RLRouletteLayer::onNextLevel(bool levelTextVisible, bool enableLoadingCircle, float loadingCirclePosY)
{
	// hides PlayingMenu's RLDifficultyNode, coins and level text
	for (int i = 1; i < 4; i++)
		playing_menu->getChildByTag(i)->setVisible(levelTextVisible);
	for (int i = 5; i < 9; i++)
		playing_menu->getChildByTag(i)->setVisible(false);

	m_loading_circle->setPositionY(loadingCirclePosY);
	if (enableLoadingCircle)
	{
		m_loading_circle->setVisible(true);
		m_loading_circle->runAction(CCRepeatForever::create(CCRotateBy::create(1.f, 360)));
	}
}

void RLRouletteLayer::onEnter()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority, true);
	CCLayer::onEnter();
}

CCMenuItemSpriteExtra* RLRouletteLayer::getDifficultyButton(GJDifficulty difficulty)
{
	return static_cast<CCMenuItemSpriteExtra*>(main_menu->getChildByTag(static_cast<int>(difficulty)));
}

void RLRouletteLayer::setDifficultyButtonColor(GJDifficulty difficulty, const ccColor3B& color)
{
	const auto button = getDifficultyButton(difficulty);

	if (auto difficultyNode = typeinfo_cast<RLDifficultyNode*>(button->getNormalImage()))
		difficultyNode->setColor(color);
	else
		button->setColor(color);
}


void RLRouletteLayer::getRandomListLevel(GJDifficulty difficulty, ListFetcher::level_pair_t& level, std::string& error)
{
	int listType = rl::utils::getIndexOf(g_rouletteManager.getFromSaveContainer("selected-list-array").as_array(), true);
	std::thread getListThread;

	switch (listType)
	{
	case 0:
		getListThread = std::thread([&](GJDifficulty difficulty, ListFetcher::level_pair_t& level, std::string& error) {
			m_list_fetcher.getRandomNormalListLevel(difficulty, std::ref(level), std::ref(error));
		}, difficulty, std::ref(level), std::ref(error));
		break;
	case 1:
		getListThread = std::thread([&](ListFetcher::level_pair_t& level, std::string& error) {
			m_list_fetcher.getRandomDemonListLevel(std::ref(level), std::ref(error));
		}, std::ref(level), std::ref(error));
		break;
	case 2:
		getListThread = std::thread([&](ListFetcher::level_pair_t& level, std::string& error) {
			m_list_fetcher.getRandomChallengeListLevel(std::ref(level), std::ref(error));
		}, std::ref(level), std::ref(error));
		break;
	case 3:
		getListThread = std::thread([&](ListFetcher::level_pair_t& level, std::string& error) {
			m_list_fetcher.getRandomGDListLevel(g_rouletteManager.gdListID, std::ref(level), std::ref(error));
		}, std::ref(level), std::ref(error));
		break;
	default:
		m_level = {};
		return;
	}

	// manually set to true because sometimes the thread detaches too late
	m_list_fetcher.is_fetching = true;
	this->scheduleUpdate();
	getListThread.detach();
}

CCMenuItemSpriteExtra* RLRouletteLayer::createDifficultyButton(
	GJDifficulty difficulty,
	CCNode* sprite,
	CCPoint point,
	float scale,
	bool isDemon,
	bool visible
) {
	sprite->setScale(scale);
	sprite->setID("sprite-node");
	auto button = CCMenuItemSpriteExtra::create(
		sprite,
		this,
		menu_selector(RLRouletteLayer::onDifficultyButton)
	);
	if (!g_rouletteManager.getArrayState(
			g_rouletteManager.getFromSaveContainer("difficulty-array"),
			rl::constants::diff_to_idx.at(difficulty)
	)) {
		if (auto diff = typeinfo_cast<RLDifficultyNode*>(sprite))
			diff->setColor({ 125, 125, 125 });
		else
			button->setColor({ 125, 125, 125 });
	}
	button->setPosition(point);
	button->setTag(static_cast<int>(difficulty));
	button->setVisible(visible);
	main_menu->addChild(button);

	return button;
}
