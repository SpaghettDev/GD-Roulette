#pragma once

#include "base/BaseCustomAlertLayer.hpp"

#include "GameState.hpp"

class RLStatsAlertLayer : public BaseCustomAlertLayer
{
public:
	static RLStatsAlertLayer* create(const GameState&);
	bool init(const GameState&);

	void onLevelsPlayedButton(cocos2d::CCObject*);

	void onClose(cocos2d::CCObject*) override;
	void keyDown(cocos2d::enumKeyCodes) override;
};
