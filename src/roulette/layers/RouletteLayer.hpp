#pragma once
#include "../../pch.hpp"
#include "../../listfetcher/ListFetcher.hpp"
#include "../../custom_layers/CustomLayer.hpp"
#include "../../custom_layers/ConfirmationLayer.hpp"

class RouletteLayer : public CustomLayer
{
private:
	inline static bool m_plus_button_state = false;
	const std::map<std::string, int> m_difficulty_to_tag{
		{ "Easy", 4 },
		{ "Normal", 5 },
		{ "Hard", 6 },
		{ "Harder", 7 },
		{ "Insane", 8 },
		{ "Easy Demon", 12 },
		{ "Medium Demon", 13 },
		{ "Hard Demon", 9 },
		{ "Insane Demon", 10 },
		{ "Extreme Demon", 11 }
	};

	gd::LoadingCircle* m_loading_circle{};
	ConfirmationAlertLayer* m_confirmation_layer{};

	static nlohmann::json m_level;
	static ListFetcher m_list_fetcher;
	static curlResponse m_list_fetcher_response;

public:
	static RouletteLayer* create();
	bool init();

	void update(float);

	void onClose(CCObject*);
	void onInfoButton(CCObject*);
	void onDifficultyChosen(CCObject*);
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


	CCMenu* m_pMainMenu{};
	CCMenu* m_pPlayingMenu{};
	CCMenu* m_pFinishedMenu{};
	CCMenu* m_pErrorMenu{};

private:
	void getRandomListLevel(int, nlohmann::json&, curlResponse&);
	gd::CCMenuItemSpriteExtra* createDifficultyButton(int, CCNode*, CCPoint, float, bool = false, bool = true);
};
