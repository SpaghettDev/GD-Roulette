#pragma once
#include "../../custom_layers/base/BaseCustomLayer.hpp"
#include "../../custom_layers/IntegerInputLayer.hpp"
#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class RouletteInfoLayer : public BaseCustomLayer
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
	CCMenuItemToggler* createToggler(int, const char*, CCPoint, bool = true);

	void destroyLayerChildren();
};
