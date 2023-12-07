#pragma once
#include "../../pch.hpp"
#include "../../layers/CustomLayer.hpp"

class RouletteInfoLayer : public CustomLayer
{
public:
	static RouletteInfoLayer* create();
	bool init();

	void onClose(CCObject*);
	void onToggleButton(CCObject*);
	void onSkipsButton(CCObject*);

private:
	gd::CCMenuItemToggler* createToggler(int, const char*, CCPoint, bool = true);

	void destroyLayerChildren();
};
