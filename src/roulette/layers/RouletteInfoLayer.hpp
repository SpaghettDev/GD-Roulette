#pragma once
#include "../../pch.hpp"
#include "../../custom_layers/CustomLayer.hpp"
#include "../../custom_layers/IntegerInputLayer.hpp"

class RouletteInfoLayer : public CustomLayer
{
private:
	IntegerInputLayer* m_integer_input_layer{};

public:
	static RouletteInfoLayer* create();
	bool init();

	void onClose(CCObject*);
	void onToggleButton(CCObject*);
	void onNumSkipsButton(CCObject*);

private:
	gd::CCMenuItemToggler* createToggler(int, const char*, CCPoint, bool = true);

	void destroyLayerChildren();
};
