#pragma once
#include "../pch.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

namespace CreatorLayer
{
	inline bool(__thiscall* init)(gd::CreatorLayer*);
	bool __fastcall initHook(gd::CreatorLayer*);
}
