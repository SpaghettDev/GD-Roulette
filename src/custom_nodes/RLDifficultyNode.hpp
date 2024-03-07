#pragma once
// #include <Geode/cocos/base_nodes/CCNode.h>
// #include <Geode/cocos/sprite_nodes/CCSprite.h>
// #include <Geode/cocos/include/ccTypes.h>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class RLDifficultyNode : public CCNodeRGBA
{
private:
	struct DifficultyInfo
	{
		GJDifficulty difficulty;
		bool featured;
		bool epic;

		bool operator==(const DifficultyInfo&) const = default;
	} m_difficulty_info;

	CCSprite* m_difficulty_sprite;
	CCSprite* m_featured_sprite;
	CCSprite* m_epic_sprite;
	ccColor3B m_color;

public:
	static RLDifficultyNode* create(GJDifficulty, bool = false, bool = false);
	bool init(GJDifficulty, bool = false, bool = false);

	// TODO: fix this setColor fuckery
	void setColor(const ccColor3B&) override;

	void setDifficulty(GJDifficulty, bool = false, bool = false);
	const DifficultyInfo& getDifficultyInfo() const { return m_difficulty_info; };
};
