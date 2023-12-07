#pragma once
#include "../pch.hpp"
#include "CustomLayer.hpp"

class IntegerInputLayer : public CustomLayer
{
public:
	static IntegerInputLayer* create();
	bool init();

	void onClose(CCObject*);
	void onLeftButton(CCObject*);
	void onRightButton(CCObject*);
};
