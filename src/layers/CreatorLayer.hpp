#pragma once
#include "../pch.hpp"
#include "../roulette/layer/RouletteLayer.hpp"

namespace CreatorLayer
{
	inline bool(__thiscall* init)(gd::CreatorLayer* self);
	bool __fastcall initHook(gd::CreatorLayer* self);

	class ButtonsClass final : public gd::FLAlertLayer, public cocos2d::CCTextFieldDelegate, public gd::FLAlertLayerProtocol
	{
	public:
		void onRouletteButton(CCObject* sender)
		{
			RouletteLayer::create()->show();
		}
	};
}
