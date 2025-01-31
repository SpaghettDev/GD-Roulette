#pragma once

#include <string_view>
#include <optional>
#include <functional>

#include "base/BaseCustomAlertLayer.hpp"

class RLIntegerInputAlertLayer;

struct RLIntegerInputData
{
	std::string_view title;
	std::string_view input_text;
	int min_value; // inclusive
	int fallback_value;
	int max_value; // inclusive
	std::optional<int> starting_value;
	int max_length;
	bool show_arrows;
	std::function<void(RLIntegerInputAlertLayer*)> onFinish;
};

class RLIntegerInputAlertLayer : public BaseCustomAlertLayer
{
public:
	static RLIntegerInputAlertLayer* create(const RLIntegerInputData&);
	bool init(const RLIntegerInputData&);

	void onClose(cocos2d::CCObject*) override;
	void onLeftButton(cocos2d::CCObject*);
	void onRightButton(cocos2d::CCObject*);

	void keyDown(cocos2d::enumKeyCodes) override;

	int m_integer;

private:
	CCTextInputNode* m_input_node;
	RLIntegerInputData m_iili;
};
