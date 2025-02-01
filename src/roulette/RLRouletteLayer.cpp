#include "RLRouletteLayer.hpp"

#include <matjson.hpp>

#include <rtrp/objects/LevelObject.hpp>

#include "../listfetcher/ListFetcher.hpp"
#include "RLRouletteInfoAlertLayer.hpp"
#include "../custom_layers/RLDifficultySelectAlertLayer.hpp"
#include "../custom_nodes/RLDifficultyNode.hpp"
#include "../custom_nodes/RLLoadingCircle.hpp"
#include "../custom_nodes/RLRepeatingBackground.hpp"
#include "../custom_nodes/RLProgressBar.hpp"
#include "RouletteManager.hpp"
#include "DataManager.hpp"
#include "utils.hpp"

using namespace geode::prelude;


RLRouletteLayer::RLRouletteLayer()
	: main_bg{ nullptr },
	info_menu{ nullptr },
	main_menu{ nullptr },
	playing_menu{ nullptr },
	finished_menu{ nullptr },
	error_menu{ nullptr },
	m_loading_circle{ nullptr },
	m_roulette_info_layer{ nullptr },
	m_confirmation_layer{ nullptr },
	m_demon_select_layer{ nullptr },
	m_stats_layer{ nullptr },
	m_selected_difficulty{ GJDifficulty::Easy },
	m_selected_demon_difficulty{ GJDifficulty::Demon },
	m_level{ geode::Err("Unintialized level") }
{
	ListFetcher::get().setFinishedFetchingCallback([&] {
		finishLevelRoulette();
	});
}

RLRouletteLayer* RLRouletteLayer::create()
{
	auto* ret = new RLRouletteLayer();

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
	if (!BaseCustomLayer::init("RL_sideArt_001.png"_spr)) return false;

	auto& rlm = RouletteManager::get();
	auto winSize = CCDirector::sharedDirector()->getWinSize();

	m_selected_difficulty = rlm.previousDifficulty;
	m_selected_demon_difficulty = rlm.previousDemonDifficulty;

	this->setID("RLRouletteLayer");

	main_bg = CCLayer::create();
	info_menu = CCMenu::create();
	main_menu = CCMenu::create();
	playing_menu = CCMenu::create();
	finished_menu = CCMenu::create();
	error_menu = CCMenu::create();

	main_bg->setID("main-bg");
	main_menu->setID("main-menu");
	info_menu->setID("info-menu");

	playing_menu->setID("playing-menu");
	playing_menu->setVisible(false);

	finished_menu->setID("finished-menu");
	finished_menu->setVisible(false);

	error_menu->setID("error-menu");
	error_menu->setVisible(false);

	this->addChild(main_bg, -1);
	this->addChild(info_menu, 11);
	this->addChild(main_menu, 11);
	this->addChild(playing_menu, 11);
	this->addChild(finished_menu, 11);
	this->addChild(error_menu, 11);


	auto title = CCLabelBMFont::create("GD Level Roulette", "goldFont.fnt");
	title->setPosition({ winSize.width / 2.f, winSize.height - 20.f });
	title->limitLabelWidth(340.f, 1.f, 0.f);
	title->setID("title-label");
	this->addChild(title);


	auto repeatingBg = RLRepeatingBackground::create(
		fmt::format("game_bg_{:02}_001.png", rl::utils::randomNumber(1, 59)).c_str()
	);
	repeatingBg->setColor({ 40, 40, 40 });
	repeatingBg->setID("repeating-bg");
	m_backgroundMenu->addChild(repeatingBg, -1);


	info_menu->setPosition({ .0f, .0f });
	info_menu->setAnchorPoint({ .0f, .0f });

	auto infoButtonSprite = CCSprite::create("RL_optionsBtn_001.png"_spr);
	infoButtonSprite->setScale(.48f);
	auto infoButton = CCMenuItemSpriteExtra::create(
		infoButtonSprite,
		this,
		menu_selector(RLRouletteLayer::onInfoButton)
	);
	infoButton->setPosition({ winSize.width - 20.f, 25.f });
	infoButton->setSizeMult(1.2f);
	infoButton->setID("info-button");
	info_menu->addChild(infoButton);

	if (!Mod::get()->hasSavedValue("show-options-sprite"))
	{
		auto optionsSprite = CCSprite::create("RL_optionsTxt_001.png"_spr);
		optionsSprite->setPosition({ -28.f, 40.f });
		optionsSprite->setID("options-sprite");
		infoButton->addChild(optionsSprite);
	}

	auto statsButtonSprite = CCSprite::create("RL_statsBtn_001.png"_spr);
	statsButtonSprite->setScale(.48f);
	auto statsButton = CCMenuItemSpriteExtra::create(
		statsButtonSprite,
		this,
		menu_selector(RLRouletteLayer::onStatsButton)
	);
	statsButton->setPosition({ winSize.width - 20.f, winSize.height - 20.f });
	statsButton->setVisible(false);
	statsButton->setID("stats-button");
	info_menu->addChild(statsButton);


	// GJListLayer type border
	{
		auto borderTitle = CCLabelBMFont::create("Select Difficulty", "bigFont.fnt");
		borderTitle->setScale(.75f);
		borderTitle->setPosition({ .0f, 104.f });
		borderTitle->setID("border-title");
		main_bg->addChild(borderTitle);

		{
			auto background1 = CCSpriteBatchNode::create("ListBorder.png"_spr);
			background1->setID("background-1");
			main_bg->addChild(background1);

			auto topBorderSprite = CCSprite::createWithSpriteFrameName("RL_listBorderTop_001.png"_spr);
			topBorderSprite->setScale(.925f);
			topBorderSprite->setPosition({ .0f, 100.f });
			topBorderSprite->setID("top-border");
			background1->addChild(topBorderSprite, -1);

			auto bottomBorderSprite = CCSprite::createWithSpriteFrameName("RL_listBorderBottom_001.png"_spr);
			bottomBorderSprite->setScale(.925f);
			bottomBorderSprite->setPosition({ .0f, -100.f });
			bottomBorderSprite->setID("bottom-border");
			background1->addChild(bottomBorderSprite, -1);

			auto sideBorderSpriteLeft = CCSprite::createWithSpriteFrameName("RL_listBorderSide_001.png"_spr);
			sideBorderSpriteLeft->setScaleX(1.375f);
			sideBorderSpriteLeft->setScaleY(2.7f);
			sideBorderSpriteLeft->setPosition({ -167.f, .0f });
			sideBorderSpriteLeft->setID("side-border-left");
			background1->addChild(sideBorderSpriteLeft, -2);

			auto sideBorderSpriteRight = CCSprite::createWithSpriteFrameName("RL_listBorderSide_001.png"_spr);
			sideBorderSpriteRight->setScaleX(1.375f);
			sideBorderSpriteRight->setScaleY(2.7f);
			sideBorderSpriteRight->setFlipX(true);
			sideBorderSpriteRight->setPosition({ 167.f, .0f });
			sideBorderSpriteRight->setID("side-border-right");
			background1->addChild(sideBorderSpriteRight, -2);

			auto bg = CCSprite::createWithSpriteFrameName("RL_listBorderBackground_001.png"_spr);
			bg->setColor({ 40, 40, 40 });
			bg->setScaleX(4.2f);
			bg->setScaleY(2.3f);
			bg->setID("background-1");
			background1->addChild(bg, -3);
		}

		{
			auto background2 = CCSpriteBatchNode::create("ListBorder.png"_spr);
			background2->setVisible(false);
			background2->setID("background-2");
			main_bg->addChild(background2);

			auto topBorderSprite = CCSprite::createWithSpriteFrameName("RL_listBorderTop_002.png"_spr);
			topBorderSprite->setScale(.925f);
			topBorderSprite->setPosition({ .0f, 100.f });
			topBorderSprite->setID("top-border");
			background2->addChild(topBorderSprite, -1);

			auto bottomBorderSprite = CCSprite::createWithSpriteFrameName("RL_listBorderBottom_002.png"_spr);
			bottomBorderSprite->setScale(.925f);
			bottomBorderSprite->setPosition({ .0f, -100.f });
			bottomBorderSprite->setID("bottom-border");
			background2->addChild(bottomBorderSprite, -1);

			auto sideBorderSpriteLeft = CCSprite::createWithSpriteFrameName("RL_listBorderSide_001.png"_spr);
			sideBorderSpriteLeft->setScaleX(1.375f);
			sideBorderSpriteLeft->setScaleY(2.7f);
			sideBorderSpriteLeft->setPosition({ -167.f, .0f });
			sideBorderSpriteLeft->setID("side-border-left");
			background2->addChild(sideBorderSpriteLeft, -2);

			auto sideBorderSpriteRight = CCSprite::createWithSpriteFrameName("RL_listBorderSide_001.png"_spr);
			sideBorderSpriteRight->setScaleX(1.375f);
			sideBorderSpriteRight->setScaleY(2.7f);
			sideBorderSpriteRight->setFlipX(true);
			sideBorderSpriteRight->setPosition({ 167.f, .0f });
			sideBorderSpriteRight->setID("side-border-right");
			background2->addChild(sideBorderSpriteRight, -2);

			auto bg = CCSprite::createWithSpriteFrameName("RL_listBorderBackground_002.png"_spr);
			bg->setColor({ 40, 40, 40 });
			bg->setScaleX(21.5f);
			bg->setScaleY(1.1f);
			bg->setID("background-2");
			background2->addChild(bg, -3);
		}

		main_bg->ignoreAnchorPointForPosition(false);
		main_bg->setPosition({ winSize.width, winSize.height });
	}


	main_menu->addChild(
		createDifficultyButton(
			GJDifficulty::Easy,
			CCSprite::createWithSpriteFrameName("difficulty_01_btn_001.png"),
			{ -100.f, 40.f }, 1.2f
		)
	);
	main_menu->addChild(
		createDifficultyButton(
			GJDifficulty::Normal,
			CCSprite::createWithSpriteFrameName("difficulty_02_btn_001.png"),
			{ .0f, 40.f }, 1.2f
		)
	);
	main_menu->addChild(
		createDifficultyButton(
			GJDifficulty::Hard,
			CCSprite::createWithSpriteFrameName("difficulty_03_btn_001.png"),
			{ 100.f, 40.f }, 1.2f
		)
	);
	main_menu->addChild(
		createDifficultyButton(
			GJDifficulty::Harder,
			CCSprite::createWithSpriteFrameName("difficulty_04_btn_001.png"),
			{ -100.f, -30.f }, 1.2f
		)
	);
	main_menu->addChild(
		createDifficultyButton(
			GJDifficulty::Insane,
			CCSprite::createWithSpriteFrameName("difficulty_05_btn_001.png"),
			{ .0f, -30.f }, 1.2f
		)
	);
	main_menu->addChild(
		createDifficultyButton(
			GJDifficulty::Demon,
			RLDifficultyNode::create(
				m_selected_difficulty >= GJDifficulty::Demon
					? m_selected_demon_difficulty
					: static_cast<GJDifficulty>(-2)
			),
			{ 100.f, -30.f }, 1.2f
		)
	);
	auto plusButton = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
		this,
		menu_selector(RLRouletteLayer::onPlusButton)
	);
	plusButton->setPosition({ 140.f, -25.f });
	plusButton->setSizeMult(1.2f);
	plusButton->setID("demon-plus-button");
	plusButton->setVisible(
		rl::utils::getIndexOf(
			DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().asVector(), true
		) == 0 &&
		m_selected_difficulty >= GJDifficulty::Demon
	);
	main_menu->addChild(plusButton);

	auto startButtonText = CCLabelBMFont::create("Start", "bigFont.fnt");
	startButtonText->setPosition({ 46.f, 18.f });
	startButtonText->setScale(.725f);
	auto startButton = CCMenuItemSpriteExtra::create(
		CCSprite::create("RL_longBtn_001.png"_spr),
		this,
		menu_selector(RLRouletteLayer::onStartButton)
	);
	startButton->setPosition({ .0f, -136.f });
	startButton->setID("start-button");
	startButton->addChild(startButtonText);
	main_menu->addChild(startButton);


	m_loading_circle = RLLoadingCircle::create();
	m_loading_circle->positionCenter();
	m_loading_circle->fadeOut();
	this->addChild(m_loading_circle);


	auto levelNameButtonLabel = CCLabelBMFont::create("LevelName", "bigFont.fnt");
	levelNameButtonLabel->limitLabelWidth(100.f, 1.f, .1f);
	levelNameButtonLabel->setID("button-label");
	auto levelNameButton = CCMenuItemSpriteExtra::create(
		levelNameButtonLabel,
		this,
		menu_selector(RLRouletteLayer::onLevelInfo)
	);
	levelNameButton->setSizeMult(1.2f);
	levelNameButton->setPosition({ 50.f, 60.f });
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
	levelCreatorButton->setPosition({ 50.f, 37.f });
	levelCreatorButton->setID("level-creator-button");
	levelCreatorButton->setTag(2);
	playing_menu->addChild(levelCreatorButton);

	auto levelPlayButtonSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
	levelPlayButtonSprite->setScale(.9f);
	auto levelPlayButtonItem = CCMenuItemSpriteExtra::create(
		levelPlayButtonSprite,
		this,
		menu_selector(RLRouletteLayer::onPlayButton)
	);
	levelPlayButtonItem->setPosition({ -100.f, -40.f });
	levelPlayButtonItem->setID("play-button");
	levelPlayButtonItem->setTag(4);
	playing_menu->addChild(levelPlayButtonItem);

	auto difficultyNode = RLDifficultyNode::create({
		GJDifficulty::Normal, GJFeatureState::None
	});
	difficultyNode->setPosition({ -100.f, 40.f });
	difficultyNode->setScale(1.5f);
	difficultyNode->setID("difficulty-node");
	difficultyNode->setTag(5);
	playing_menu->addChild(difficultyNode);

	auto attemptText = CCLabelBMFont::create(
		fmt::format("Attempt {}", rlm.gameState.levelAttempts).c_str(),
		"goldFont.fnt"
	);
	attemptText->setPosition({ 50.f, -27.f });
	attemptText->setScale(.8f);
	attemptText->setID("attempt-count-label");
	playing_menu->addChild(attemptText);

	auto percentageText = CCLabelBMFont::create(
		fmt::format("{}%", rlm.currentPercentageGoal).c_str(),
		"goldFont.fnt"
	);
	percentageText->setPosition({ 50.f, -60.f });
	percentageText->setScale(.8f);
	percentageText->setID("percentage-text");
	playing_menu->addChild(percentageText);

	auto percentageBg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png");
	percentageBg->setPosition({ 20.f, -77.f });
	percentageBg->setContentSize({ 60.f, 30.f });
	percentageBg->setAnchorPoint({ .0f, .0f });
	percentageBg->setColor({ 0, 0, 0 });
	percentageBg->setOpacity(100);
	percentageBg->setID("percentage-bg");
	playing_menu->addChild(percentageBg, -1);

	auto progressBar = RLProgressBar::create(rlm.gameState.levelPercentage, false);
	progressBar->setPosition({ .0f, 102.f });
	progressBar->setColor({ 255, 255, 0 });
	progressBar->setScale(.9f);
	progressBar->setID("progress-bar");
	playing_menu->addChild(progressBar);

	auto skipButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Skip", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, 1.f),
		this,
		menu_selector(RLRouletteLayer::onSkipButton)
	);
	skipButton->setPosition({ -90.f, -135.f });
	skipButton->setID("skip-button");
	playing_menu->addChild(skipButton);

	auto nextButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Next", 50, true, "bigFont.fnt", "GJ_button_01.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onNextButton)
	);
	nextButton->setPosition({ .0f, -135.f });
	nextButton->setID("next-button");
	playing_menu->addChild(nextButton);

	auto resetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onResetButton)
	);
	resetButton->setPosition({ 90.f, -135.f });
	resetButton->setID("reset-button");
	playing_menu->addChild(resetButton);


	auto ggText = CCLabelBMFont::create("GG!", "goldFont.fnt");
	ggText->setPosition({ .0f, 65.f });
	ggText->setID("gg-label");
	finished_menu->addChild(ggText);

	auto ggSkipsUsedText = CCLabelBMFont::create("Skips Used: ", "bigFont.fnt");
	ggSkipsUsedText->setPosition({ .0f, 35.f });
	ggSkipsUsedText->setID("skips-label");
	finished_menu->addChild(ggSkipsUsedText);

	auto ggNumLevelsText = CCLabelBMFont::create("Levels Played: ", "bigFont.fnt");
	ggNumLevelsText->setPosition({ .0f, 7.f });
	ggNumLevelsText->setID("levels-played-label");
	finished_menu->addChild(ggNumLevelsText);

	auto ggTotalAttempts = CCLabelBMFont::create("Total Attempts: ", "bigFont.fnt");
	ggTotalAttempts->limitLabelWidth(100.f, 1.f, .85f);
	ggTotalAttempts->setPosition({ .0f, -20.f });
	ggTotalAttempts->setID("total-attempts-label");
	finished_menu->addChild(ggTotalAttempts);

	auto levelsPlayedButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("View Levels", 100, true, "bigFont.fnt", "GJ_button_02.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onLevelsPlayedButton)
	);
	levelsPlayedButton->setPosition({ -60.f, -140.f });
	levelsPlayedButton->setID("levels-played-button");
	finished_menu->addChild(levelsPlayedButton);

	auto finishedResetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onResetButton)
	);
	finishedResetButton->setPosition({ 80.f, -140.f });
	finishedResetButton->setID("reset-button");
	finished_menu->addChild(finishedResetButton);


	auto errorText = CCLabelBMFont::create("An error has occurred", "bigFont.fnt");
	errorText->setPosition({ .0f, 105.f });
	errorText->setColor({ 255, 0, 0 });
	errorText->setScale(.725f);
	errorText->setID("title-label");
	error_menu->addChild(errorText);

	auto errorReasonText = TextArea::create(
		"ErrorReason",
		"chatFont.fnt",
		1.f, 300.f, { .5f, .5f }, 20.f, false
	);
	errorReasonText->setPosition({ .0f, 15.f });
	errorReasonText->setID("reason-label");
	error_menu->addChild(errorReasonText);

	auto refreshButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Refresh", 50, true, "bigFont.fnt", "GJ_button_01.png", .0f, .9f),
		this,
		menu_selector(RLRouletteLayer::onRefreshButton)
	);
	refreshButton->setPosition({ -70.f, -140.f });
	refreshButton->setID("refresh-button");
	error_menu->addChild(refreshButton);

	auto errorResetButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Reset", 50, true, "bigFont.fnt", "GJ_button_06.png", .0f, .8f),
		this,
		menu_selector(RLRouletteLayer::onResetButton)
	);
	errorResetButton->setID("reset-button");
	error_menu->addChild(errorResetButton);
	errorResetButton->setPosition({ 70.f, -140.f });


	onListChanged();


	if (rlm.gameState.levelPercentage == 100 && rlm.gameState.hasReachedGoal)
	{
		main_bg->getChildByID("border-title")->setVisible(false);
		main_bg->getChildByID("background-1")->setVisible(false);
		main_bg->getChildByID("background-2")->setVisible(true);
		main_menu->setVisible(false);

		onNextButton(nullptr);
	}
	else if (rlm.isPaused)
	{
		rlm.isPlaying = true;
		rlm.isPaused = false;

		rlm.gameTimer.setElapsedTime(rlm.gameState.playTime);
		rlm.gameTimer.start();

		main_bg->getChildByID("border-title")->setVisible(false);
		main_bg->getChildByID("background-1")->setVisible(false);
		main_bg->getChildByID("background-2")->setVisible(true);
		main_menu->setVisible(false);

		setupForNextLevel(false, true);

		rl::utils::createNotificationToast(
			this, "Resuming round of Roulette...", 1.f, 85.f
		);

		ListFetcher::get().getLevelInfo(rlm.gameState.levelID, m_level);
	}

	return true;
}

void RLRouletteLayer::onClose(CCObject*)
{
	auto& rlm = RouletteManager::get();

	if (rlm.isPlaying)
	{
		m_confirmation_layer = RLConfirmationAlertLayer::create({
			"Woah there!",
			"Would you like to <cr>quit</c> or <co>pause</c> the roulette?",
			[&](auto cl) {
				rlm.gameTimer.pause();
				rlm.saveState();
				rlm.isPlaying = false;
				rlm.isPaused = true;

				onClose(nullptr);
			},
			[&](auto cl) {
				rlm.reset();

				onClose(nullptr);
			},
			"Pause", "Quit"
		});
		if (m_confirmation_layer)
			m_confirmation_layer->show();
	}
	else
	{
		this->setKeypadEnabled(false);
		this->setKeyboardEnabled(false);
		rlm.rouletteLayer = nullptr;

		CCDirector::sharedDirector()->popSceneWithTransition(
			.5f, PopTransition::kPopTransitionFade
		);
	}
}

void RLRouletteLayer::onInfoButton(CCObject*)
{
	if (RouletteManager::get().isPlaying)
		return rl::utils::createNotificationToast(
			this, "no", .5f, 85.f
		);

	if (!Mod::get()->hasSavedValue("show-options-sprite"))
	{
		info_menu->getChildByID("info-button")->getChildByID("options-sprite")->setVisible(false);
		Mod::get()->setSavedValue<bool>("show-options-sprite", false);
	}

	if ((m_roulette_info_layer = RLRouletteInfoAlertLayer::create()))
		m_roulette_info_layer->show();
}

void RLRouletteLayer::onStatsButton(CCObject*)
{
	auto& rlm = RouletteManager::get();

	rlm.gameState.playTime = rlm.gameTimer.getElapsedTime();

	if ((m_stats_layer = RLStatsAlertLayer::create(rlm.gameState)))
		m_stats_layer->show();
}

void RLRouletteLayer::onDifficultyButton(CCObject* sender)
{
	if (rl::utils::getIndexOf(DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().asVector(), true) != 0)
		return;

	auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
	auto difficulty = static_cast<GJDifficulty>(sender->getTag());
	auto array = DataManager::get<DMArrayKey::DIFFICULTY_ARRAY>();
	int prevIdx = rl::utils::getIndexOf(array.asVector(), true);

	array.set(prevIdx, false);
	array.set(rl::constants::save_data::DIFF_TO_IDX.at(difficulty), true);
	RouletteManager::get().previousDifficulty = difficulty;
	m_selected_difficulty = difficulty;

	static_cast<RLDifficultyNode*>(
		getDifficultyButton(GJDifficulty::Demon)->getChildByID("sprite-node")
	)->setDifficulty(
		m_selected_difficulty == GJDifficulty::Demon
			? RouletteManager::get().previousDemonDifficulty
			: static_cast<GJDifficulty>(-2)
	);
	getDifficultyButton(rl::constants::save_data::IDX_TO_DIFF.at(prevIdx))->setColor({ 125, 125, 125 });
	getDifficultyButton(difficulty)->setColor({ 255, 255, 255 });

	main_menu->getChildByID("demon-plus-button")->setVisible(
		m_selected_difficulty >= GJDifficulty::Demon
	);
}

void RLRouletteLayer::onStartButton(CCObject*)
{
	auto& rlm = RouletteManager::get();

	main_menu->setVisible(false);
	main_bg->getChildByID("border-title")->setVisible(false);
	main_bg->getChildByID("background-1")->setVisible(false);
	main_bg->getChildByID("background-2")->setVisible(true);

	rlm.gameState.startTime = rl::utils::getUnixEpoch();
	rlm.gameTimer.start();

	getRandomListLevel();
	setupForNextLevel(false, true);

	rlm.isPlaying = true;
}

void RLRouletteLayer::onPlusButton(CCObject*)
{
	m_demon_select_layer = RLDifficultySelectAlertLayer::create({
		"Demon Filter",
		{ static_cast<GJDifficulty>(-2), GJDifficulty::DemonEasy, GJDifficulty::DemonMedium, GJDifficulty::Demon, GJDifficulty::DemonInsane, GJDifficulty::DemonExtreme },
		m_selected_demon_difficulty,
		[&](GJDifficulty currentDifficulty, GJDifficulty previousDifficulty)
		{
			auto array = DataManager::get<DMArrayKey::DEMON_DIFFICULTY_ARRAY>();

			m_selected_demon_difficulty = currentDifficulty;

			array.set(rl::constants::save_data::DEMON_DIFF_TO_IDX.at(previousDifficulty), false);
			array.set(rl::constants::save_data::DEMON_DIFF_TO_IDX.at(currentDifficulty), true);
			RouletteManager::get().previousDemonDifficulty = currentDifficulty;

			static_cast<RLDifficultyNode*>(
				getDifficultyButton(GJDifficulty::Demon)->getChildByID("sprite-node")
			)->setDifficulty(currentDifficulty);
		}
	});
	if (m_demon_select_layer)
	{
		m_demon_select_layer->m_scene = this;
		m_demon_select_layer->show();
	}
}

void RLRouletteLayer::onLevelInfo(CCObject* sender)
{
	const auto& level = m_level.unwrap();
	auto textButton = static_cast<CCMenuItemSpriteExtra*>(sender);
	std::string text;

	switch (textButton->getTag())
	{
		case 1: text = level.first.name; break;
		case 2: text = level.second.name; break;
		case 3: text = fmt::format("{}", level.first.levelID); break;
		default: text = "[invalid]"; break;
	}

	clipboard::write(text);
	rl::utils::createNotificationToast(this, "Copied to Clipboard", .5f, 85.f);
}

void RLRouletteLayer::onPlayButton(CCObject*)
{
	if (ListFetcher::get().is_fetching)
		return;

	LevelInfoLayer* layer;

	if (m_level.isOkAnd([](const auto& lvl) { return lvl.first.levelID != 0; }))
	{
		const auto& lvl = m_level.unwrap();

		layer = LevelInfoLayer::create(rl::utils::createLevelFromResponse(lvl), false);
		layer->downloadLevel();
	}
	else
		layer = LevelInfoLayer::create(GJGameLevel::create(), true);

	auto scene = CCScene::create();
	scene->addChild(layer);

	CCDirector::sharedDirector()->pushScene(
		CCTransitionFade::create(.5f, scene)
	);
}

void RLRouletteLayer::onNextButton(CCObject*)
{
	if (ListFetcher::get().is_fetching)
		return;

	auto& rlm = RouletteManager::get();

	if (rlm.gameState.levelPercentage == 100)
	{
		auto winSize = CCDirector::sharedDirector()->getWinSize();

		rlm.gameTimer.pause();

		setupForNextLevel();

		static_cast<CCLabelBMFont*>(finished_menu->getChildByID("skips-label"))->setString(
			fmt::format("Skips Used: {}", rlm.gameState.skipsUsed).c_str()
		);
		static_cast<CCLabelBMFont*>(finished_menu->getChildByID("levels-played-label"))->setString(
			fmt::format("Levels Played: {}", rlm.gameState.numLevels).c_str()
		);
		static_cast<CCLabelBMFont*>(finished_menu->getChildByID("total-attempts-label"))->setString(
			fmt::format("Total Attempts: {}", rlm.gameState.totalAttempts).c_str()
		);

		info_menu->getChildByID("stats-button")->setVisible(true);
		info_menu->getChildByID("stats-button")->runAction(
			CCSequence::create(
				CCFadeOut::create(.25f),
				CCMoveTo::create(.0f, { winSize.width / 2.f, 100.f }),
				CCFadeIn::create(.25f),
				nullptr
			)
		);

		playing_menu->setVisible(false);
		finished_menu->setVisible(true);
	}
	else if (rlm.gameState.hasReachedGoal)
	{
		rlm.gameState.hasReachedGoal = false;
		rlm.currentPercentageGoal = rlm.gameState.levelPercentageGoal;
		rlm.gameState.levelPercentage = 0;
		rlm.gameState.levelAttempts = 0;

		setupForNextLevel(false, true, 40.f);

		static_cast<CCLabelBMFont*>(
			playing_menu->getChildByID("percentage-text")
		)->setString(fmt::format("{}%", rlm.currentPercentageGoal).c_str());
		static_cast<CCLabelBMFont*>(
			playing_menu->getChildByID("attempt-count-label")
		)->setString(fmt::format("Attempt {}", rlm.gameState.levelAttempts).c_str());

		if (m_level.isOkAnd([](auto&& level) { return level.first.levelID != 0; }))
			rlm.gameState.playedLevels.emplace_back(m_level.unwrap().first.levelID);

		getRandomListLevel();
	}
	else
		rl::utils::createNotificationToast(this, fmt::format("You need to get at least {}%!", rlm.currentPercentageGoal), .5f, 85.f);
}

void RLRouletteLayer::onRefreshButton(CCObject*)
{
	RouletteManager::get().gameState.hasReachedGoal = true;

	error_menu->setVisible(false);
	static_cast<RLRepeatingBackground*>(
		m_backgroundMenu->getChildByID("repeating-bg")
	)->start();

	onNextButton(nullptr);
}

void RLRouletteLayer::onLevelsPlayedButton(CCObject*)
{
	auto list = GJLevelList::create();

	auto listData = rl::utils::createGJListDictFromLevels(
		RouletteManager::get().gameState.playedLevels,
		RouletteManager::get().gameState.startTime,
		true
	);

	list->dataLoaded(listData.get());
	list->m_difficulty = static_cast<int>(
		m_selected_difficulty == GJDifficulty::Demon
			? m_selected_demon_difficulty
			: m_selected_difficulty
	);
	list->m_isEditable = true;

	auto lll = LevelListLayer::create(list);
	auto scene = CCScene::create();
	scene->addChild(lll);

	CCDirector::sharedDirector()->pushScene(
		CCTransitionFade::create(
			.5f,
			scene
		)
	);

	// onBack bug fix
	lll->m_levelList->m_listType = GJLevelType::Default;
}

void RLRouletteLayer::onResetButton(CCObject*)
{
	if (ListFetcher::get().is_fetching)
		return;

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	RouletteManager::get().reset();

	m_level = geode::Err("No level fetched");

	setupForNextLevel();

	main_menu->setVisible(true);
	playing_menu->setVisible(false);
	finished_menu->setVisible(false);
	error_menu->setVisible(false);

	main_bg->getChildByID("border-title")->setVisible(true);
	main_bg->getChildByID("background-1")->setVisible(true);
	main_bg->getChildByID("background-2")->setVisible(false);
	info_menu->getChildByID("info-button")->setVisible(true);
	info_menu->getChildByID("stats-button")->setPosition(
		{ winSize.width - 20.f, winSize.height - 20.f }
	);
	info_menu->getChildByID("stats-button")->setVisible(false);

	static_cast<CCLabelBMFont*>(playing_menu->getChildByID("percentage-text"))->setString(
		fmt::format("{}%", RouletteManager::get().currentPercentageGoal).c_str()
	);

	static_cast<RLProgressBar*>(
		playing_menu->getChildByID("progress-bar")
	)->setProgress(.0f);

	main_menu->getChildByID("demon-plus-button")->setPositionY(-20.f);
	main_menu->getChildByID("start-button")->setVisible(true);

	static_cast<RLRepeatingBackground*>(
		m_backgroundMenu->getChildByID("repeating-bg")
	)->start();

	m_loading_circle->positionCenter();
}

void RLRouletteLayer::onSkipButton(CCObject*)
{
	if (ListFetcher::get().is_fetching)
		return;

	auto& rlm = RouletteManager::get();

	if (rlm.gameState.levelPercentage == 100 || rlm.gameState.hasReachedGoal)
	{
		rl::utils::createNotificationToast(this, "Skip not used, you have already reached the goal!", .5f, 85.f);

		onNextButton(nullptr);

		return;
	}

	if (rlm.gameState.skipsUsed < Mod::get()->getSettingValue<int64_t>("max-skips"))
	{
		rlm.gameState.skipsUsed++;
		rlm.gameState.hasReachedGoal = false;
		rlm.currentPercentageGoal = rlm.gameState.levelPercentageGoal;

		setupForNextLevel(false, true, 40.f);

		static_cast<CCLabelBMFont*>(
			playing_menu->getChildByID("attempt-count-label")
		)->setString("Attempt 0");

		if (rlm.gameState.playedLevels.size())
			rlm.gameState.playedLevels.pop_back();

		getRandomListLevel();
	}
	else
		rl::utils::createNotificationToast(this, "You don't have any skips left!", .5f, 85.f);
}

void RLRouletteLayer::finishLevelRoulette()
{
	if (m_level.isErr())
	{
		setupForNextLevel();

		static_cast<TextArea*>(error_menu->getChildByID("reason-label"))->setString(m_level.unwrapErr());

		playing_menu->setVisible(false);
		error_menu->setVisible(true);

		static_cast<RLRepeatingBackground*>(
			m_backgroundMenu->getChildByID("repeating-bg")
		)->stop();

		return;
	}

	setupForNextLevel(true, false, 40.f);
	main_menu->setVisible(false);
	info_menu->getChildByID("info-button")->setVisible(false);
	info_menu->getChildByID("stats-button")->setVisible(true);

	const auto& [level, creator] = m_level.unwrap();
	RouletteManager::get().gameState.levelID = level.levelID;

	static_cast<CCLabelBMFont*>(
		playing_menu->getChildByID("level-name-button")->getChildByID("button-label")
	)->setString(level.name.c_str());
	static_cast<CCLabelBMFont*>(
		playing_menu->getChildByID("level-creator-button")->getChildByID("button-label")
	)->setString(fmt::format("by {}", creator.name).c_str());

	static_cast<RLDifficultyNode*>(playing_menu->getChildByID("difficulty-node"))->setDifficulty({
		rl::utils::getDifficultyFromResponse(level), rl::utils::getFeatureStateFromResponse(level)
	});
	playing_menu->getChildByID("difficulty-node")->setVisible(true);
	playing_menu->getChildByID("difficulty-node")->setPositionY(40.f);

	playing_menu->setVisible(true);

	RouletteManager::get().saveState();
}

void RLRouletteLayer::setupForNextLevel(bool levelTextVisible, bool enableLoadingCircle, float loadingCirclePosYOffset)
{
	// hides PlayingMenu's RLDifficultyNode and level text
	for (int i = 1; i < 3; i++)
		playing_menu->getChildByTag(i)->setVisible(levelTextVisible);
	playing_menu->getChildByTag(5)->setVisible(false);

	if (enableLoadingCircle)
		m_loading_circle->show();
	else
		m_loading_circle->stopAndHide();
	m_loading_circle->setPositionY(
		CCDirector::sharedDirector()->getWinSize().height / 2.f + loadingCirclePosYOffset
	);
}

void RLRouletteLayer::onListChanged()
{
	const int selectedList = rl::utils::getIndexOf(
		DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().asVector(),
		true
	);

	for (const auto difficulty : {
			GJDifficulty::Easy, GJDifficulty::Normal, GJDifficulty::Hard,
			GJDifficulty::Harder, GJDifficulty::Insane, GJDifficulty::Demon
	}) {
		auto button = getDifficultyButton(difficulty);

		button->setColor({ 125, 125, 125 });
		button->setEnabled(selectedList == 0);
	}

	GJDifficulty visualDifficulty;

	switch (selectedList)
	{
		case 0:
			onDifficultyButton(
				getDifficultyButton(m_selected_difficulty)
			);
		break;

		case 1:
			visualDifficulty = GJDifficulty::Demon;
		break;

		case 2:
			visualDifficulty = GJDifficulty::Insane;
		break;

		case 3:
			visualDifficulty = GJDifficulty::Easy;
		break;

		default:
			break;
	};

	if (selectedList != 0)
	{
		getDifficultyButton(visualDifficulty)->setColor({ 255, 255, 255 });

		static_cast<RLDifficultyNode*>(
			getDifficultyButton(GJDifficulty::Demon)->getChildByID("sprite-node")
		)->setDifficulty(
			visualDifficulty == GJDifficulty::Demon
				? GJDifficulty::DemonExtreme
				: static_cast<GJDifficulty>(-2)
		);

		main_menu->getChildByID("demon-plus-button")->setVisible(false);
	}
}

// set roulette button exclamation mark
void RLRouletteLayer::onExitTransitionDidStart()
{
	BaseCustomLayer::onExitTransitionDidStart();

	if (auto nextScene = static_cast<CCScene*>(CCDirector::sharedDirector()->m_pobScenesStack->firstObject()))
	{
		CCMenuItemSpriteExtra* rouletteButton = nullptr;

		if (Loader::get()->isModLoaded("ninxout.redash"))
		{
			if (auto menuLayer = nextScene->getChildByID("MenuLayer"))
			{
				rouletteButton = static_cast<CCMenuItemSpriteExtra*>(
					menuLayer->querySelector(
						fmt::format("right-side-menu > {}", "roulette-button"_spr)
					)
				);
			}
		}
		else if (auto creatorLayer = nextScene->getChildByID("CreatorLayer"))
		{
			bool hasBigButton = Mod::get()->getSettingValue<bool>("use-big-button");

			rouletteButton = static_cast<CCMenuItemSpriteExtra*>(
				creatorLayer->querySelector(fmt::format(
					"{} > {}",
					hasBigButton ? "creator-buttons-menu" : "center-left-menu"_spr,
					"roulette-button"_spr
				))
			);
		}

		// where tf my button go??
		if (!rouletteButton)
			return;

		if (RouletteManager::get().isPaused)
			RouletteManager::get().addExclamationMark(rouletteButton);
		else if (auto exMark = rouletteButton->getChildByID("exclamation-mark"_spr))
			exMark->removeFromParentAndCleanup(true);
	}
}

void RLRouletteLayer::onEnterTransitionDidFinish()
{
	BaseCustomLayer::onEnterTransitionDidFinish();

	auto& rlm = RouletteManager::get();

	if (!rlm.isPlaying || !rlm.hasEnteredPlayLayer)
		return;

	auto progressBar = static_cast<RLProgressBar*>(
		playing_menu->getChildByID("progress-bar")
	);

	if (static_cast<int>(progressBar->getProgress()) != rlm.gameState.levelPercentage)
	{
		progressBar->setProgress(rlm.gameState.levelPercentage);
		progressBar->playFillingAnimation();
	}

	static_cast<CCLabelBMFont*>(
		playing_menu->getChildByID("attempt-count-label")
	)->setString(fmt::format("Attempt {}", rlm.gameState.levelAttempts).c_str());

	rlm.hasEnteredPlayLayer = false;
}


void RLRouletteLayer::getRandomListLevel()
{
	int listType = rl::utils::getIndexOf(
		DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().asVector(),
		true
	);

	auto& listFetcher = ListFetcher::get();

	switch (listType)
	{
	case 0:
		listFetcher.getRandomNormalListLevel(
			m_selected_difficulty == GJDifficulty::Demon
				? m_selected_demon_difficulty
				: m_selected_difficulty,
			m_level
		);
		break;

	case 1:
		listFetcher.getRandomDemonListLevel(m_level);
		break;

	case 2:
		listFetcher.getRandomChallengeListLevel(m_level);
		break;

	case 3:
		listFetcher.getRandomGDListLevel(RouletteManager::get().gdListID, m_level);
		break;

	default:
		m_level = geode::Err("Invalid list type");
		return;
	}
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
	button->setCascadeColorEnabled(true);
	if (
		rl::utils::getIndexOf(DataManager::get<DMArrayKey::SELECTED_LIST_ARRAY>().asVector(), true) != 0 ||
		m_selected_difficulty != difficulty
		)
		button->setColor({ 125, 125, 125 });
	button->setPosition(point);
	button->setTag(static_cast<int>(difficulty));
	button->setVisible(visible);

	return button;
}
