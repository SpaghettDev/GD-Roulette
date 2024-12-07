#pragma once
#include <string_view>
#include <vector>
#include <functional>

#include <Geode/Enums.hpp>

#include "base/BaseCustomAlertLayer.hpp"

class RLDifficultySelectLayer;

struct RLDifficultySelectInfo
{
	std::string_view title;
	std::vector<GJDifficulty> difficulties;
	GJDifficulty selectedDifficulty;
	std::function<void(GJDifficulty, GJDifficulty)> onSelectDifficulty;
	std::function<void(RLDifficultySelectLayer*)> onOk = nullptr;
};


class RLDifficultySelectLayer : public BaseCustomAlertLayer
{
public:
	CCMenu* difficulty_buttons_menu{};

	static RLDifficultySelectLayer* create(const RLDifficultySelectInfo&);
	bool init(const RLDifficultySelectInfo&);

	void onClose(CCObject*) override;
	void onDemonButton(CCObject*);

	void keyDown(enumKeyCodes) override;
	void registerWithTouchDispatcher() override;

private:
	RLDifficultySelectInfo m_dsi;
};
