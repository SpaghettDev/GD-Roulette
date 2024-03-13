#pragma once
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/actions/CCActionInterval.h>

class RLLoadingCircle : public cocos2d::CCSprite
{
public:
	static RLLoadingCircle* create();

	bool init() override;

	void positionCenter();

	void fadeIn();
	void fadeOut();
	void startRotate();
	void stopRotate();

	void show();
	void stopAndHide();

private:
	enum ACTION_TAG : int
	{
		FADE_IN,
		FADE_OUT,
		ROTATE
	};
};
