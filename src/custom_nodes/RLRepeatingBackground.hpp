#pragma once

#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

#include <Geode/cocos/sprite_nodes/CCSprite.h>

class RLRepeatingBackground : public cocos2d::CCLayerRGBA
{
public:
	static RLRepeatingBackground* create(const char*);
	bool init(const char*);

	virtual void setColor(const cocos2d::ccColor3B&) override;
	virtual void update(float dt) override;

	void start();
	void stop();

private:
	constexpr static float SCROLL_SPEED = 30.f;

	cocos2d::CCSprite* bg1;
	cocos2d::CCSprite* bg2;
	cocos2d::CCSprite* bg3;
};
