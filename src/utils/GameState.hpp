#pragma once

#include <vector>

static_assert(true); // clangd bug fix

#pragma pack(push, 1)
struct GameState
{
	inline static constexpr std::uint16_t VERSION = 10;

	/**
	 * @brief Version of the GameState struct
	 *
	 * Increments of 1 is patch (default value changed, etc...)
	 * 10 is major change (new member)
	 */
	std::uint16_t version = VERSION;

	std::uint64_t startTime = 0;
	std::uint64_t playTime = 0;

	std::uint64_t levelID = 0;
	std::uint16_t skipsUsed = 0;
	std::uint32_t numLevels = 0;
	std::uint8_t levelPercentage = 0;
	std::uint8_t levelPercentageGoal = 1;
	std::uint32_t levelAttempts = 0;
	std::uint32_t totalAttempts = 0;
	bool hasReachedGoal = false;

	std::vector<int> playedLevels;

	void reset()
	{
		startTime = 0;
		playTime = 0;

		levelID = 0;
		skipsUsed = 0;
		numLevels = 0;
		levelPercentage = 0;
		levelPercentageGoal = 1;
		levelAttempts = 0;
		totalAttempts = 0;
		hasReachedGoal = false;

		playedLevels.clear();
	}
};
#pragma pack(pop)

template<>
class std::hash<GameState>
{
public:
	std::size_t operator()(const GameState& gs) const
	{
		std::size_t hash = 0;

		hash = std::hash<std::uint64_t>{}(gs.startTime);
		hash = hash * 31 + std::hash<std::uint64_t>{}(gs.playTime);
		hash = hash * 31 + std::hash<std::uint64_t>{}(gs.levelID);
		hash = hash * 31 + std::hash<std::uint16_t>{}(gs.skipsUsed);
		hash = hash * 31 + std::hash<std::uint32_t>{}(gs.numLevels);
		hash = hash * 31 + std::hash<std::uint8_t>{}(gs.levelPercentage);
		hash = hash * 31 + std::hash<std::uint8_t>{}(gs.levelPercentageGoal);
		hash = hash * 31 + std::hash<std::uint32_t>{}(gs.levelAttempts);
		hash = hash * 31 + std::hash<std::uint32_t>{}(gs.totalAttempts);
		hash = hash * 31 + std::hash<bool>{}(gs.hasReachedGoal);

		for (const auto& level : gs.playedLevels)
			hash = hash * 31 + std::hash<int>{}(level);

		return hash;
	}
};
