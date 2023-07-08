#pragma once
#include "../../pch.hpp"
#include "../../listfetcher/ListFetcher.hpp"

class CustomLayer : public gd::FLAlertLayer
{
public:
	CCPoint alertSize{};
	gd::CCMenuItemSpriteExtra* closeBtn{};

	bool createBasics(CCPoint, SEL_MenuHandler, float = 1, const ccColor4B& = { 0x00, 0x00, 0x00, 0x4B });
	void createTitle(std::string, float = .75f, float = 1);
	gd::CCMenuItemSpriteExtra* createButton(const char*, CCPoint, SEL_MenuHandler, int = -1, float = 1.0f, float = 1.2f);
	virtual void onClose(CCObject*) = 0;
	void keyDown(enumKeyCodes);
	void keyBackClicked();
};


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

class RouletteInfoLayer : public CustomLayer
{
public:
	static RouletteInfoLayer* create();
	bool init();

	void onClose(CCObject*);
	void onToggleButton(CCObject*);
	void onSkipsButton(CCObject*);

private:
	gd::CCMenuItemToggler* createToggler(int, const char*, CCPoint, bool = true);

	void destroyLayerChildren();
};

class IntegerInputLayer : public CustomLayer
{
public:
	static IntegerInputLayer* create();
	bool init();

	void onClose(CCObject*);
	void onLeftButton(CCObject*);
	void onRightButton(CCObject*);
};
