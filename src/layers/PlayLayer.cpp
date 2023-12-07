#include "PlayLayer.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"

float previousPosition = .0f, delta = -1.f;

bool __fastcall PlayLayer::initHook(gd::PlayLayer* self, void*, gd::GJGameLevel* level)
{
	delta = .0f;
	previousPosition = .0f;

	return init(self, level);
}

void __fastcall PlayLayer::updateHook(gd::PlayLayer* self, void*, float dt)
{
	if (self->m_pPlayer1->getPositionX() != previousPosition)
		delta += dt;

	previousPosition = self->m_pPlayer1->getPositionX();

	update(self, dt);
}

void __fastcall PlayLayer::resetLevelHook(gd::PlayLayer* self, void*)
{
	delta = .0f;
	previousPosition = .0f;

	resetLevel(self);
}

void __fastcall PlayLayer::destroyPlayerHook(gd::PlayLayer* self, void*, gd::PlayerObject* player, gd::GameObject* obj)
{
	if (
		float percentage = (player->getPositionX() / self->m_levelLength) * 100.f;
		RouletteManager.isPlayingRoulette &&
		self->m_level->levelID == RouletteManager.lastLevelID &&
		!self->m_isPracticeMode &&
		percentage >= RouletteManager.levelPercentageGoal
		) {
		if (delta > .2f && !self->m_isDead)
		{
			RouletteManager.hasFinishedPreviousLevel = true;
			RouletteManager.lastLevelPercentage = percentage;
			RouletteManager.levelPercentageGoal = RouletteManager.lastLevelPercentage + 1.f;
			RouletteManager.numLevels++;
			// TODO: wait a few seconds then pause
			self->pauseGame(false);
		}
	}

	destroyPlayer(self, player, obj);
}

gd::GameSoundManager* __fastcall PlayLayer::levelCompleteHook(gd::PlayLayer* self)
{
	if (
		RouletteManager.isPlayingRoulette &&
		self->m_level->levelID == RouletteManager.lastLevelID &&
		!self->m_isPracticeMode
		) {
		if (delta > .2f && !self->m_isDead)
		{
			RouletteManager.hasFinishedPreviousLevel = true;
			RouletteManager.lastLevelPercentage = 100;
			RouletteManager.levelPercentageGoal++;
			RouletteManager.numLevels++;
		}
	}

	return levelComplete(self);
}
