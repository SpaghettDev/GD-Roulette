#pragma once
#include "../pch.hpp"

namespace PlayLayer
{
	inline bool(__thiscall* init)(gd::PlayLayer*, gd::GJGameLevel*);
	bool __fastcall initHook(gd::PlayLayer*, void*, gd::GJGameLevel*);

	inline void(__thiscall* update)(gd::PlayLayer*, float);
	void __fastcall updateHook(gd::PlayLayer*, void*, float);

	inline void(__thiscall* resetLevel)(gd::PlayLayer*);
	void __fastcall resetLevelHook(gd::PlayLayer*, void*);

	inline void(__thiscall* destroyPlayer)(gd::PlayLayer*, gd::PlayerObject*, gd::GameObject*);
	void __fastcall destroyPlayerHook(gd::PlayLayer*, void*, gd::PlayerObject*, gd::GameObject*);

	inline gd::GameSoundManager*(__thiscall* levelComplete)(gd::PlayLayer*);
	gd::GameSoundManager* __fastcall levelCompleteHook(gd::PlayLayer*);
}
