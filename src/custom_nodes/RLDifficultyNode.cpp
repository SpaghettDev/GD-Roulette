#include "RLDifficultyNode.hpp"
#include "constants.hpp"

#include <Geode/binding/GameToolbox.hpp>

using namespace geode::prelude;

// taken from GJDifficultySprite::updateFeatureState
// "30a-1a1.3a0.2a20a90a0a10a5a20a20a0a0a8a0a0a0a4a1a0a0a0a0a1a0a1a0a1a0a1a1a0a0a0a0a0.784314a0a1a0a1a0a0.27a0a0.27a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0.25a0a0a0a0a0a0a0a0a0a0a0"
static cocos2d::ParticleStruct mythicParticles{
	.TotalParticles = 30,
	.Duration = -1,
	.Life = 1.3,
	.LifeVar = 0.2,
	.EmissionRate = 20,
	.Angle = 90,
	.AngleVar = 0,
	.Speed = 10,
	.SpeedVar = 5,
	.PosVarX = 20,
	.PosVarY = 20,
	.GravityX = 0,
	.GravityY = 0,
	.RadialAccel = 8,
	.RadialAccelVar = 0,
	.TangentialAccel = 0,
	.TangentialAccelVar = 0,
	.StartSize = 4,
	.StartSizeVar = 1,
	.StartSpin = 0,
	.StartSpinVar = 0,
	.StartColorR = 0,
	.StartColorVarR = 0,
	.StartColorG = 1,
	.StartColorVarG = 0,
	.StartColorB = 1,
	.StartColorVarB = 0,
	.StartColorA = 1,
	.StartColorVarA = 0,
	.EndSize = 1,
	.EndSizeVar = 1,
	.EndSpin = 0,
	.EndSpinVar = 0,
	.EndColorR = 0,
	.EndColorVarR = 0,
	.EndColorG = 0.784314,
	.EndColorVarG = 0,
	.EndColorB = 1,
	.EndColorVarB = 0,
	.EndColorA = 1,
	.EndColorVarA = 0,
	.FadeInTime = 0.27,
	.FadeInTimeVar = 0,
	.FadeOutTime = 0.27,
	.FadeOutTimeVar = 0,
	.StartRadius = 0,
	.StartRadiusVar = 0,
	.EndRadius = 0,
	.EndRadiusVar = 0,
	.RotatePerSecond = 0,
	.RotatePerSecondVar = 0,
	.EmitterMode = 0,
	.PositionType = 2,
	.isBlendAdditive = true,
	.startSpinEqualToEndSpin = false,
	.rotationIsDir = false,
	.dynamicRotationIsDir = false,
	.customParticleIdx = 0,
	.uniformColorMode = false,
	.frictionPos = 0,
	.frictionPosVar = 0.25,
	.respawn = 0,
	.respawnVar = 0,
	.orderSensitive = false,
	.startSizeEqualToEndSize = false,
	.startRadiusEqualToEndRadius = false,
	.startRGBVarSync = false,
	.endRGBVarSync = false,
	.frictionSize = 0,
	.frictionSizeVar = 0,
	.frictionRot = 0,
	.frictionRotVar = 0,
	.sFrame = "particle_00_001.png"
};


RLDifficultyNode* RLDifficultyNode::create(const DifficultyInfo& di)
{
	auto* ret = new RLDifficultyNode();

	if (ret && ret->init(di))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

RLDifficultyNode* RLDifficultyNode::create(GJDifficulty difficulty)
{
	return create({ difficulty, GJFeatureState::None });
}

bool RLDifficultyNode::init(const DifficultyInfo& di)
{
	if (!CCNodeRGBA::init()) return false;

	m_difficulty_info = di;

	this->setCascadeColorEnabled(true);
	this->setCascadeOpacityEnabled(true);
	m_will_set_anchor = true;
	this->setAnchorPoint({ .0f, .0f });
	m_will_set_anchor = false;


	m_difficulty_sprite = CCSprite::createWithSpriteFrameName(
		rl::constants::sprites::DIFFICULTY_TO_SPRITE.at(m_difficulty_info.difficulty).data()
	);
	m_difficulty_sprite->setID("difficulty-sprite");
	this->addChild(m_difficulty_sprite);

	this->setContentSize(m_difficulty_sprite->getContentSize());

	switch (m_difficulty_info.feature_state)
	{
		case GJFeatureState::Featured:
		case GJFeatureState::Epic:
		case GJFeatureState::Legendary:
		case GJFeatureState::Mythic:
			m_feature_sprite = CCSprite::createWithSpriteFrameName(
				rl::constants::sprites::FEATURE_STATE_TO_SPRITE.at(m_difficulty_info.feature_state).data()
			);
		break;

		default:
			m_feature_sprite = nullptr;
		break;
	}

	if (m_feature_sprite)
	{
		m_feature_sprite->setPosition(m_difficulty_sprite->getPosition());
		m_feature_sprite->setID("feature-sprite");
		this->addChild(m_feature_sprite, -1);
	}

	return true;
}

// basically RLDifficultyNode::init but without the call to CCNodeRGBA::init and some extra checks
void RLDifficultyNode::setDifficulty(const DifficultyInfo& di)
{
	if (m_difficulty_info == di) return;

	if (m_difficulty_info.difficulty != di.difficulty)
	{
		m_difficulty_sprite->removeFromParent();

		m_difficulty_sprite = CCSprite::createWithSpriteFrameName(
			rl::constants::sprites::DIFFICULTY_TO_SPRITE.at(di.difficulty).data()
		);
		m_difficulty_sprite->setID("difficulty-sprite");
		this->addChild(m_difficulty_sprite);
	}

	if (m_difficulty_info.feature_state != di.feature_state)
	{
		if (m_feature_sprite)
			m_feature_sprite->removeFromParent();

		switch (di.feature_state)
		{
			case GJFeatureState::Featured:
			case GJFeatureState::Epic:
			case GJFeatureState::Legendary:
			case GJFeatureState::Mythic:
				m_feature_sprite = CCSprite::createWithSpriteFrameName(
					rl::constants::sprites::FEATURE_STATE_TO_SPRITE.at(di.feature_state).data()
				);
				m_feature_sprite->setID("feature-sprite");
				this->addChild(m_feature_sprite, -1);
			break;

			default:
				m_feature_sprite = nullptr;
			break;
		}

		if (di.feature_state == GJFeatureState::Mythic)
		{
			m_mythic_particles = GameToolbox::particleFromStruct(mythicParticles, nullptr, false);

			m_mythic_particles->setScale(.9f);
			m_mythic_particles->setPosition(m_feature_sprite->getPosition());
			this->addChild(m_mythic_particles, -1);

			// rob you are truly genius
			int startOffset = 5;
			do
			{
				m_mythic_particles->update(.15f);
				--startOffset;
			}
			while (startOffset);
		}
		else if (m_mythic_particles)
			m_mythic_particles->removeFromParentAndCleanup(true);
	}

	m_difficulty_info = di;

	// force cascade colors again after changing sprite
	this->setColor(this->getColor());
}

void RLDifficultyNode::setDifficulty(GJDifficulty difficulty)
{
	setDifficulty({ difficulty, GJFeatureState::None });
}

// replacing bad code with even more bad code :D
void RLDifficultyNode::setAnchorPoint(const CCPoint& anchorPoint)
{
	if (m_will_set_anchor)
		CCNodeRGBA::setAnchorPoint(anchorPoint);
}
