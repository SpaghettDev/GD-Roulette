#pragma once

static_assert(true); // clangd bug fix

#pragma pack(push, 1)
struct GameState
{
	int levelID = 0;
	int skipsUsed = 0;
	int numLevels = 0;
	int levelPercentage = 0;
	int levelPercentageGoal = 1;
	bool hasReachedGoal = false;
};
#pragma pack(pop)
