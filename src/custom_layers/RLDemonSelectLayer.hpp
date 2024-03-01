#pragma once
#include "base/BaseCustomAlertLayer.hpp"

class RLDemonSelectLayer;

struct RLDemonSelectLayerInfo
{
	GJDifficulty selectedDifficulty;
	std::function<void(GJDifficulty, GJDifficulty)> onSelectDifficulty;
	std::function<void(RLDemonSelectLayer*)> onOk = nullptr;
};


class RLDemonSelectLayer : public BaseCustomAlertLayer
{
public:
	static RLDemonSelectLayer* create(const RLDemonSelectLayerInfo&);
	bool init(const RLDemonSelectLayerInfo&);

	void onClose(CCObject*) override;
	void onDemonButton(CCObject*);

	void keyDown(enumKeyCodes) override;

private:
	RLDemonSelectLayerInfo m_dsli;
};
