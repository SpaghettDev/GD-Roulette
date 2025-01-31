#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

class BaseCustomLayer : public cocos2d::CCLayer
{
public:
	bool init(const char*);

	virtual void onClose(cocos2d::CCObject*) = 0;
	virtual void keyDown(cocos2d::enumKeyCodes) override;
	virtual void keyBackClicked() override;

public:
	cocos2d::CCMenu* m_backMenu;
	cocos2d::CCMenu* m_sideArtMenu;
	cocos2d::CCMenu* m_backgroundMenu;

	CCMenuItemSpriteExtra* m_backBtn;
};
