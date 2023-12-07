#pragma once
#include "../../pch.hpp"
#include "../../listfetcher/ListFetcher.hpp"
#include "../../layers/CustomLayer.hpp"

class RouletteLayer : public CustomLayer
{
private:
	inline static bool isPlusButtonToggled = false;
	std::map<std::string, int> difficultyToTag{
		{ "Easy", 103 },
		{ "Normal", 104 },
		{ "Hard", 105 },
		{ "Harder", 106 },
		{ "Insane", 107 },
		{ "Easy Demon", 108 },
		{ "Medium Demon", 109 },
		{ "Hard Demon", 110 },
		{ "Insane Demon", 111 },
		{ "Extreme Demon", 112 }
	};

	gd::LoadingCircle* levelLoadingCircle{};
	CCSprite* levelEpicSprite{};
	CCSprite* levelFeaturedSprite{};

	static nlohmann::json level;
	static ListFetcher listFetcher;
	static curlResponse listFetcherResponse;

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
	void onNextButton(CCObject*);
	void onSkipButton(CCObject*);
	void onResetButton(CCObject*);

	void finishLevelRoulette();

private:
	void getRandomListLevel(int, nlohmann::json&, curlResponse&);
	gd::CCMenuItemSpriteExtra* createDifficultyButton(int, CCNode*, CCPoint, float, bool = false, bool = true);
};
