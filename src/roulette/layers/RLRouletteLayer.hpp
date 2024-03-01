#pragma once
#include "../../listfetcher/ListFetcher.hpp"
#include "RLRouletteInfoLayer.hpp"
#include "../../custom_layers/base/BaseCustomAlertLayer.hpp"
#include "../../custom_layers/RLConfirmationAlertLayer.hpp"
#include "../../custom_layers/RLDemonSelectLayer.hpp"

#include <Geode/Bindings.hpp>

#include <rtrp/objects/LevelObject.hpp>

using namespace geode::prelude;

class RLRouletteLayer : public BaseCustomAlertLayer
{
private:
	LoadingCircle* m_loading_circle{};
	RLRouletteInfoLayer* m_roulette_info_layer{};
	RLConfirmationAlertLayer* m_confirmation_layer{};
	RLDemonSelectLayer* m_demon_select_layer{};

	GJDifficulty m_selected_difficulty;
	GJDifficulty m_selected_demon_difficulty;

	static ListFetcher m_list_fetcher;
	static ListFetcher::level_pair_t m_level;
	static std::string m_list_fetcher_error;

public:
	CCMenu* main_menu{};
	CCMenu* playing_menu{};
	CCMenu* finished_menu{};
	CCMenu* error_menu{};

	static RLRouletteLayer* create();
	bool init() override;

	void update(float) override;

	void onClose(CCObject*) override;
	void onInfoButton(CCObject*);
	void onDifficultyButton(CCObject*);
	void onStartButton(CCObject*);
	void onPlusButton(CCObject*);
	void onLevelInfo(CCObject*);
	void onPlayButton(CCObject*);
	void onSkipButton(CCObject*);
	void onNextButton(CCObject*);
	void onResetButton(CCObject*);

	void finishLevelRoulette();
	void onNextLevel(bool = false, bool = false, float = -175.f);

	void onEnter() override;
	CCMenuItemSpriteExtra* getDifficultyButton(GJDifficulty);
	void setDifficultyButtonColor(GJDifficulty, const ccColor3B&);

private:
	void getRandomListLevel(GJDifficulty, ListFetcher::level_pair_t&, std::string&);
	CCMenuItemSpriteExtra* createDifficultyButton(GJDifficulty, CCNode*, CCPoint, float, bool = false, bool = true);
};
