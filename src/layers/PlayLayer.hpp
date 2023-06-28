#pragma once
#include "../pch.hpp"

namespace PlayLayer
{
	inline bool(__thiscall* init)(gd::PlayLayer* self, gd::GJGameLevel* level);
	bool __fastcall initHook(gd::PlayLayer* self, void*, gd::GJGameLevel* level);

	inline void(__thiscall* update)(gd::PlayLayer* self, float dt);
	void __fastcall updateHook(gd::PlayLayer* self, void*, float dt);

	inline void(__thiscall* resetLevel)(gd::PlayLayer* self);
	void __fastcall resetLevelHook(gd::PlayLayer* self, void*);

	inline void(__thiscall* destroyPlayer)(gd::PlayLayer* self, gd::PlayerObject*, gd::GameObject*);
	void __fastcall destroyPlayerHook(gd::PlayLayer* self, void*, gd::PlayerObject* player, gd::GameObject* obj);

	inline gd::GameSoundManager*(__thiscall* levelComplete)(gd::PlayLayer* self);
	gd::GameSoundManager* __fastcall levelCompleteHook(gd::PlayLayer* self);
}
