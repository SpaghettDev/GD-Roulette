#pragma once
#include "../pch.hpp"

namespace LevelInfoLayer
{
	inline void(__thiscall* onBack)(gd::LevelInfoLayer*, CCObject*);
	void __fastcall onBackHook(gd::LevelInfoLayer*, void*, CCObject*);
}
