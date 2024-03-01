#pragma once
#include "base/BaseCustomAlertLayer.hpp"

class RLConfirmationAlertLayer;

struct ConfirmationAlertInfo
{
	std::string_view title;
	std::string_view text;
	std::function<void(RLConfirmationAlertLayer*)> onBtn1;
	std::function<void(RLConfirmationAlertLayer*)> onBtn2 = nullptr;
	std::string_view yesText = "YES";
	std::string_view noText = "NO";
};


class RLConfirmationAlertLayer : public BaseCustomAlertLayer
{
public:
	static RLConfirmationAlertLayer* create(const ConfirmationAlertInfo&);
	bool init(const ConfirmationAlertInfo&);

	void onClose(CCObject*) override;
	void onYesButton(CCObject*);
	void onNoButton(CCObject*);

	void keyDown(enumKeyCodes) override {};

private:
	ConfirmationAlertInfo m_cli;
};
