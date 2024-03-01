#pragma once
#include "../../custom_layers/base/BaseCustomAlertLayer.hpp"
#include "../../custom_layers/RLIntegerInputLayer.hpp"
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class RLRouletteInfoLayer : public BaseCustomAlertLayer
{
private:
	RLIntegerInputLayer* m_integer_input_layer{};

public:
	static RLRouletteInfoLayer* create();
	bool init();

	void onClose(CCObject*);
	void onToggleButton(CCObject*);
	void onNumSkipsButton(CCObject*);
	void onInfoIcon(CCObject*);
	void onIDButton(CCObject*);

private:
	CCMenuItemToggler* createToggler(int, const std::string&, const char*, CCPoint, bool = true);
};
