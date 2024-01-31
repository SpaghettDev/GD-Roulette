#pragma once
#include "../pch.hpp"
#include "base/BaseCustomLayer.hpp"

class ConfirmationAlertLayer;

struct ConfirmationAlertInfo
{
	std::string_view title;
	std::string_view text;
	std::function<void(ConfirmationAlertLayer*)> onYes;
	std::function<void(ConfirmationAlertLayer*)> onNo = nullptr;
	std::string_view yesText = "YES";
	std::string_view noText = "NO";

	friend void swap(ConfirmationAlertInfo& lhs, ConfirmationAlertInfo& rhs)
	{
		std::swap(lhs.title, rhs.title);
		std::swap(lhs.text, rhs.text);
		std::swap(lhs.onYes, rhs.onYes);
		std::swap(lhs.onNo, rhs.onNo);
		std::swap(lhs.yesText, rhs.yesText);
		std::swap(lhs.noText, rhs.noText);
	}
};


class ConfirmationAlertLayer : public BaseCustomLayer
{
public:
	static ConfirmationAlertLayer* create();
	bool init();

	void onClose(CCObject*);
	void onYesButton(CCObject*);
	void onNoButton(CCObject*);

	void keyDown(enumKeyCodes) override {};

	void setup(ConfirmationAlertInfo cli) { m_tmp_cli = cli; };

private:
	static inline ConfirmationAlertInfo m_tmp_cli;
	ConfirmationAlertInfo m_cli;
};
