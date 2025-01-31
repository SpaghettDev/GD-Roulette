#pragma once

#include <string_view>
#include <vector>
#include <functional>

#include <Geode/Enums.hpp>

#include "base/BaseCustomAlertLayer.hpp"

class RLDifficultySelectAlertLayer;

struct RLDifficultySelectData
{
	std::string_view title;
	std::vector<GJDifficulty> difficulties;
	GJDifficulty selectedDifficulty;
	std::function<void(GJDifficulty, GJDifficulty)> onSelectDifficulty;
	std::function<void(RLDifficultySelectAlertLayer*)> onOk = nullptr;
};


class RLDifficultySelectAlertLayer : public BaseCustomAlertLayer
{
public:
	static RLDifficultySelectAlertLayer* create(const RLDifficultySelectData&);
	bool init(const RLDifficultySelectData&);

	void onDemonButton(cocos2d::CCObject*);

	virtual void onClose(cocos2d::CCObject*) override;
	void keyDown(cocos2d::enumKeyCodes) override;
	void registerWithTouchDispatcher() override;

private:
	cocos2d::CCMenu* difficulty_buttons_menu;
	RLDifficultySelectData m_dsi;
};
