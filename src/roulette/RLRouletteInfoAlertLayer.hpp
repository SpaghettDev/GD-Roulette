#pragma once

#include "../custom_layers/base/BaseCustomAlertLayer.hpp"
#include "../custom_layers/RLIntegerInputAlertLayer.hpp"

class RLRouletteInfoAlertLayer : public BaseCustomAlertLayer
{
public:
	static RLRouletteInfoAlertLayer* create();
	bool init();

	void onClose(cocos2d::CCObject*);
	void onToggleButton(cocos2d::CCObject*);
	void onNumSkipsButton(cocos2d::CCObject*);
	void onInfoIcon(cocos2d::CCObject*);
	void onIDButton(cocos2d::CCObject*);

private:
	CCMenuItemToggler* createToggler(int, const std::string&, const char*, cocos2d::CCPoint, bool = true);

private:
	RLIntegerInputAlertLayer* m_integer_input_layer;
};
