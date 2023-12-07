#pragma once
#include "../pch.hpp"

namespace PauseLayer
{
	inline gd::PauseLayer*(__thiscall* create)(bool p0);
	gd::PauseLayer* __fastcall createHook(bool, void*);
}
