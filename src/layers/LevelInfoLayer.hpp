#pragma once
#include "../pch.hpp"

namespace LevelInfoLayer
{
	inline bool(__thiscall* init)(gd::LevelInfoLayer*, gd::GJGameLevel*);
	bool __fastcall initHook(gd::LevelInfoLayer*, void*, gd::GJGameLevel*);

	inline void(__thiscall* onBack)(gd::LevelInfoLayer*, CCObject*);
	void __fastcall onBackHook(gd::LevelInfoLayer*, void*, CCObject*);
}
