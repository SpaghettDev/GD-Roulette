#pragma once

#include "../listfetcher/ListFetcher.hpp"
#include "../custom_layers/base/BaseCustomLayer.hpp"
#include "../custom_layers/RLConfirmationAlertLayer.hpp"
#include "../custom_layers/RLDifficultySelectAlertLayer.hpp"
#include "../custom_layers/RLStatsAlertLayer.hpp"
#include "../custom_nodes/RLLoadingCircle.hpp"
#include "RLRouletteInfoAlertLayer.hpp"

class RLRouletteLayer : public BaseCustomLayer
{
public:
	RLRouletteLayer();

	static RLRouletteLayer* create();
	virtual bool init() override;

	virtual void onClose(cocos2d::CCObject*) override;

	void onInfoButton(cocos2d::CCObject*);
	void onStatsButton(cocos2d::CCObject*);
	void onDifficultyButton(cocos2d::CCObject*);
	void onStartButton(cocos2d::CCObject*);
	void onPlusButton(cocos2d::CCObject*);
	void onLevelInfo(cocos2d::CCObject*);
	void onPlayButton(cocos2d::CCObject*);
	void onSkipButton(cocos2d::CCObject*);
	void onNextButton(cocos2d::CCObject*);
	void onRefreshButton(cocos2d::CCObject*);
	void onLevelsPlayedButton(cocos2d::CCObject*);
	void onResetButton(cocos2d::CCObject*);

	void onListChanged();

	virtual void onExitTransitionDidStart() override;
	virtual void onEnterTransitionDidFinish() override;

	inline CCMenuItemSpriteExtra* getDifficultyButton(GJDifficulty difficulty)
	{
		return static_cast<CCMenuItemSpriteExtra*>(main_menu->getChildByTag(static_cast<int>(difficulty)));
	}

public:
	cocos2d::CCLayer* main_bg;

	cocos2d::CCMenu* info_menu;
	cocos2d::CCMenu* main_menu;
	cocos2d::CCMenu* playing_menu;
	cocos2d::CCMenu* finished_menu;
	cocos2d::CCMenu* error_menu;

private:
	void finishLevelRoulette();
	void setupForNextLevel(bool = false, bool = false, float = .0f);

	void getRandomListLevel();
	CCMenuItemSpriteExtra* createDifficultyButton(GJDifficulty, cocos2d::CCNode*, cocos2d::CCPoint, float, bool = false, bool = true);

private:
	RLLoadingCircle* m_loading_circle;
	RLRouletteInfoAlertLayer* m_roulette_info_layer;
	RLConfirmationAlertLayer* m_confirmation_layer;
	RLDifficultySelectAlertLayer* m_demon_select_layer;
	RLStatsAlertLayer* m_stats_layer;

	GJDifficulty m_selected_difficulty;
	GJDifficulty m_selected_demon_difficulty;

	geode::Result<ListFetcher::level_pair_t> m_level;
};
