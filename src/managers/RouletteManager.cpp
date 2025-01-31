#include "RouletteManager.hpp"

#include "DataManager.hpp"
#include "utils.hpp"
#include "constants.hpp"

template <DMArrayKey key>
static GJDifficulty getDifficultyFromSaveContainer()
{
	int idx = rl::utils::getIndexOf(DataManager::get<key>().asVector(), true);

	if constexpr (key == DMArrayKey::DEMON_DIFFICULTY_ARRAY)
		return rl::constants::save_data::IDX_TO_DEMON_DIFF[idx];
	else
		return static_cast<GJDifficulty>(idx + 1);
}

RouletteManager::RouletteManager()
	: gdListID(DataManager::setDefaultSafe<DMMiscKey::GD_LIST_ID>()),
	gameState(DataManager::setDefaultSafe<DMMiscKey::SAVE_DATA>()),
	previousDifficulty(::getDifficultyFromSaveContainer<DMArrayKey::DIFFICULTY_ARRAY>()),
	previousDemonDifficulty(::getDifficultyFromSaveContainer<DMArrayKey::DEMON_DIFFICULTY_ARRAY>())
{
	static_cast<void>(DataManager::setDefaultSafe<DMArrayKey::DIFFICULTY_ARRAY>());
	static_cast<void>(DataManager::setDefaultSafe<DMArrayKey::DEMON_DIFFICULTY_ARRAY>());
	static_cast<void>(DataManager::setDefaultSafe<DMArrayKey::SELECTED_LIST_ARRAY>());

	if (gameState.levelID != 0)
	{
		currentPercentageGoal = gameState.levelPercentageGoal;
		isPaused = true;
	}
}

void RouletteManager::addExclamationMark(CCMenuItemSpriteExtra* button)
{
	if (button->getChildByID("exclamation-mark"_spr))
		return;

	auto exclamationSprite = cocos2d::CCSprite::createWithSpriteFrameName(
		"exMark_001.png"
	);
	exclamationSprite->setScale(.6f);
	exclamationSprite->setID("exclamation-mark"_spr);
	button->addChild(exclamationSprite);

	// ReDash doesn't add our layout after moving the button :(
	if (geode::Loader::get()->isModLoaded("ninxout.redash") && !button->getLayout())
		exclamationSprite->getParent()->setLayout(geode::AnchorLayout::create());

	exclamationSprite->setLayoutOptions(
		geode::AnchorLayoutOptions::create()
			->setAnchor(geode::Anchor::TopRight)
			->setOffset({ -5.f, -8.f })
	);
}

void RouletteManager::saveState()
{
	gameState.playTime = gameTimer.getElapsedTime();

	DataManager::set<DMMiscKey::SAVE_DATA>(gameState);
}

void RouletteManager::reset()
{
	gameState.reset();
	gameTimer.reset();
	saveState();

	isPlaying = false;
	isPaused = false;
	currentPercentageGoal = 1;
}