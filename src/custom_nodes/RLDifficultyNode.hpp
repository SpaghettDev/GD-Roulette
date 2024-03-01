#pragma once

#include <Geode/Bindings.hpp>

using namespace geode::prelude;

class RLDifficultyNode : public CCNode
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

	void setColor(const ccColor3B&);

	void setDifficulty(GJDifficulty, bool = false, bool = false);
	DifficultyInfo getDifficultyInfo() const { return m_difficulty_info; };
};
