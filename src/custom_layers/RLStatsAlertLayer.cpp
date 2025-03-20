#include "RLStatsAlertLayer.hpp"

#include <fmt/format.h>

#include <Geode/loader/Mod.hpp>
#include <Geode/binding/LevelCell.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GJLevelList.hpp>
#include <Geode/binding/LevelListLayer.hpp>
#include <Geode/binding/LoadingCircle.hpp>
#include <Geode/binding/GJListLayer.hpp>
#include <Geode/binding/BoomListView.hpp>
#include <Geode/binding/TextArea.hpp>

#include "RouletteManager.hpp"
#include "utils.hpp"

using namespace geode::prelude;

RLStatsAlertLayer* RLStatsAlertLayer::create(const GameState& gs)
{
	auto ret = new RLStatsAlertLayer();

	if (ret && ret->init(std::move(gs)))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLStatsAlertLayer::init(const GameState& gs)
{
	if (!this->createBasics({
		.contentSize = { 380.f, 250.f },
		.onClose = menu_selector(RLStatsAlertLayer::onClose),
		.bgTextureName = "RL_square01_001.png"_spr,
		.bgRect = { .0f, .0f, 94.f, 94.f }
	})) return false;

	this->setID("RLStatsAlertLayer");

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	this->createTitle("Statistics", .75f, 1.f);

	auto statsText = TextArea::create(
		fmt::format(
			"Started <cl>{}</c>\n"
			"Skips used: <cy>{}</c>/<cy>{}</c>\n"
			"Current level attempts: <cg>{}</c>\n"
			"Current level percentage: <cg>{}</c>%\n"
			"Levels played: <cg>{}</c>\n"
			"Total attempts: <cg>{}</c>\n"
			"Total playtime: <cl>{}</c>",
			rl::utils::formatUnixTimestamp(gs.startTime),
			gs.skipsUsed, Mod::get()->getSettingValue<int64_t>("max-skips"),
			gs.levelAttempts,
			gs.levelPercentage,
			gs.numLevels,
			gs.totalAttempts,
			rl::utils::formatTime(gs.playTime)
		),
		"chatFont.fnt", 1.f, 350.f, { .5f, .5f }, 20.f, false
	);
	statsText->setPosition({ winSize.width / 2.f, (winSize.height / 2.f) + 5.f });
	statsText->setID("stats-text");
	m_mainLayer->addChild(statsText);

	auto levelsPlayedButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("View Levels", 100, true, "bigFont.fnt", "GJ_button_02.png", .0f, .8f),
		this,
		menu_selector(RLStatsAlertLayer::onLevelsPlayedButton)
	);
	levelsPlayedButton->setPosition({ .0f, -90.f });
	levelsPlayedButton->setID("levels-played-button");
	m_buttonMenu->addChild(levelsPlayedButton);

	return true;
}

void RLStatsAlertLayer::onLevelsPlayedButton(CCObject*)
{
	const auto& rlm = RouletteManager::get();
	auto list = GJLevelList::create();

	const bool editable = rlm.gameState.levelPercentage == 100;

	auto listData = rl::utils::createGJListDictFromLevels(
		rlm.gameState.playedLevels,
		rlm.gameState.startTime,
		editable
	);

	list->dataLoaded(listData.get());
	list->m_difficulty = static_cast<int>(
		rlm.previousDifficulty == GJDifficulty::Demon
			? rlm.previousDemonDifficulty
			: rlm.previousDifficulty
	);
	list->m_isEditable = editable;

	auto lll = LevelListLayer::create(list);
	lll->m_editMode = editable;
	lll->m_circle->fadeAndRemove();

	// anti-cheat
	if (!editable)
	{
		static_cast<CCMenuItemSpriteExtra*>(
			lll->querySelector("button-menu > creator-name")
		)->setEnabled(false);

		for (auto& cell : CCArrayExt<LevelCell*>(lll->m_list->m_listView->m_tableView->m_contentLayer->getChildren()))
		{
			auto buttonSprite = cell->m_button->getChildByType<ButtonSprite>(0);

			buttonSprite->m_BGSprite->setColor({ 100, 100, 100 });
			buttonSprite->m_label->setColor({ 100, 100, 100 });

			cell->m_button->setEnabled(false);
			static_cast<CCMenuItemSpriteExtra*>(
				cell->m_button->getParent()->getChildByID("creator-name")
			)->setEnabled(false);
		}

		auto copyButton = static_cast<CCMenuItemSpriteExtra*>(
			lll->querySelector("right-side-menu > copy-button")
		);

		copyButton->setColor({ 100, 100, 100 });
		copyButton->setEnabled(false);
	}

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

void RLStatsAlertLayer::onClose(CCObject*)
{
	this->setKeypadEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void RLStatsAlertLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape || key == enumKeyCodes::KEY_Enter)
		onClose(nullptr);
}
