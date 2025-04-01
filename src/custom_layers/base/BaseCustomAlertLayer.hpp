#pragma once

#include <limits>
#include <string>

#include <Geode/cocos/include/ccTypes.h>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

struct BaseCustomAlertData
{
	cocos2d::CCPoint contentSize;
	cocos2d::SEL_MenuHandler onClose;
	float closeBtnScale = 1.f;
	cocos2d::ccColor4B color = { 0x00, 0x00, 0x00, 0x4B };
	const char* bgTextureName = "GJ_square01.png";
	cocos2d::CCRect bgRect = { .0f, .0f, 80.f, 80.f };
	int priority = std::numeric_limits<int>::max();
};

class BaseCustomAlertLayer : public FLAlertLayer
{
public:
	bool createBasics(const BaseCustomAlertData&);
	void createTitle(std::string, float = .75f, float = 1.f);

	virtual void onClose(cocos2d::CCObject*) = 0;
	virtual void keyDown(cocos2d::enumKeyCodes) override;
	virtual void keyBackClicked() override;

	virtual ~BaseCustomAlertLayer() override;

	cocos2d::CCPoint alertSize;
	CCMenuItemSpriteExtra* closeBtn;
	cocos2d::extension::CCScale9Sprite* bg;
};
