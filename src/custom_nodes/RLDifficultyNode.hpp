#pragma once
// #include <Geode/cocos/base_nodes/CCNode.h>
// #include <Geode/cocos/sprite_nodes/CCSprite.h>
// #include <Geode/cocos/include/ccTypes.h>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

enum class RL_FEATURE_STATE
{
	NONE = -1,
	FEATURED,
	EPIC,
	LEGENDARY,
	MYTHIC
};

class RLDifficultyNode : public CCNodeRGBA
{
private:
	struct DifficultyInfo
	{
		GJDifficulty difficulty;
		RL_FEATURE_STATE feature_state = RL_FEATURE_STATE::NONE;

		bool operator==(const DifficultyInfo&) const = default;
	} m_difficulty_info;

	CCSprite* m_difficulty_sprite;
	CCSprite* m_feature_sprite;

public:
	static RLDifficultyNode* create(const DifficultyInfo&);
	static RLDifficultyNode* create(GJDifficulty);
	bool init(const DifficultyInfo&);

	void setDifficulty(const DifficultyInfo&);
	void setDifficulty(GJDifficulty);

	const DifficultyInfo& getDifficultyInfo() const { return m_difficulty_info; };
};
