#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/include/ccTypes.h>
#include <Geode/cocos/particle_nodes/CCParticleSystemQuad.h>

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/Enums.hpp>

class RLDifficultyNode : public cocos2d::CCNodeRGBA
{
public:
	struct DifficultyInfo
	{
		GJDifficulty difficulty;
		GJFeatureState feature_state = GJFeatureState::None;

		bool operator==(const DifficultyInfo&) const = default;
	};

public:
	static RLDifficultyNode* create(const DifficultyInfo&);
	static RLDifficultyNode* create(GJDifficulty);
	bool init(const DifficultyInfo&);

	void setDifficulty(const DifficultyInfo&);
	void setDifficulty(GJDifficulty);

	virtual void setAnchorPoint(const cocos2d::CCPoint&) override;

	const DifficultyInfo& getDifficultyInfo() const { return m_difficulty_info; };

private:
	DifficultyInfo m_difficulty_info;

	cocos2d::CCSprite* m_difficulty_sprite;
	cocos2d::CCSprite* m_feature_sprite;
	cocos2d::CCParticleSystemQuad* m_mythic_particles;

	bool m_will_set_anchor;
};
