#pragma once
#include <limits>
#include <string>

#include <Geode/cocos/include/ccTypes.h>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

using namespace geode::prelude;

class BaseCustomAlertLayer : public FLAlertLayer
{
public:
	CCPoint alertSize{};
	CCMenuItemSpriteExtra* closeBtn{};

	bool createBasics(CCPoint, SEL_MenuHandler, float = 1.f, const ccColor4B& = { 0x00, 0x00, 0x00, 0x4B }, int = std::numeric_limits<int>::max());
	void createTitle(std::string, float = .75f, float = 1.f);
	CCMenuItemSpriteExtra* createButton(const char*, CCPoint, SEL_MenuHandler, int = -1, float = 1.f, float = 1.2f);
	virtual void onClose(CCObject*) = 0;
	void keyDown(enumKeyCodes);
	void keyBackClicked();
};
