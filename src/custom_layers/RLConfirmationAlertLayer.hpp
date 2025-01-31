#pragma once

#include <string_view>
#include <functional>

#include "base/BaseCustomAlertLayer.hpp"

class RLConfirmationAlertLayer;

struct RLConfirmationData
{
	std::string_view title;
	std::string_view text;
	std::function<void(RLConfirmationAlertLayer*)> onBtn1;
	std::function<void(RLConfirmationAlertLayer*)> onBtn2 = nullptr;
	std::string_view btn1Text = "YES";
	std::string_view btn2Text = "NO";
};


class RLConfirmationAlertLayer : public BaseCustomAlertLayer
{
public:
	static RLConfirmationAlertLayer* create(const RLConfirmationData&);
	bool init(const RLConfirmationData&);

	void onClose(cocos2d::CCObject*) override;
	void onYesButton(cocos2d::CCObject*);
	void onNoButton(cocos2d::CCObject*);

	void keyDown(cocos2d::enumKeyCodes) override {};

private:
	RLConfirmationData m_cli;
};
