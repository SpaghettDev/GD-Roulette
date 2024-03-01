#pragma once
#include <optional>

#include "base/BaseCustomAlertLayer.hpp"

class RLIntegerInputLayer;

struct IntegerInputInfo
{
	std::string_view title;
	std::string_view input_text;
	int min_value; // inclusive
	int fallback_value;
	int max_value; // inclusive
	std::optional<int> starting_value;
	bool show_arrows;
	std::function<void(RLIntegerInputLayer*)> onFinish;
};

class RLIntegerInputLayer : public BaseCustomAlertLayer
{
public:
	CCTextInputNode* input_node;

	static RLIntegerInputLayer* create(const IntegerInputInfo&);
	bool init(const IntegerInputInfo&);

	void onClose(CCObject*) override;
	void onLeftButton(CCObject*);
	void onRightButton(CCObject*);

	void keyDown(enumKeyCodes) override;

	int m_integer;

private:
	IntegerInputInfo m_iili;
};
