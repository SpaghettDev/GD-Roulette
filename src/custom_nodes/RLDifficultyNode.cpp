#include "RLDifficultyNode.hpp"
#include "constants.hpp"

#include <Geode/binding/GameToolbox.hpp>

using namespace geode::prelude;

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
			// taken from GJDifficultySprite::updateFeatureState
			// "30a-1a1.3a0.2a20a90a0a10a5a20a20a0a0a8a0a0a0a4a1a0a0a0a0a1a0a1a0a1a0a1a1a0a0a0a0a0.784314a0a1a0a1a0a0.27a0a0.27a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0.25a0a0a0a0a0a0a0a0a0a0a0"
			// auto particleStruct = cocos2d::ParticleStruct{
			// 	.TotalParticles = 30,
			// 	.Duration = -1,
			// 	.Life = 1.3,
			// 	.LifeVar = 0.2,
			// 	.EmissionRate = 20,
			// 	.Angle = 90,
			// 	.AngleVar = 0,
			// 	.Speed = 10,
			// 	.SpeedVar = 5,
			// 	.PosVarX = 20,
			// 	.PosVarY = 20,
			// 	.GravityX = 0,
			// 	.GravityY = 0,
			// 	.RadialAccel = 8,
			// 	.RadialAccelVar = 0,
			// 	.TangentialAccel = 0,
			// 	.TangentialAccelVar = 0,
			// 	.StartSize = 4,
			// 	.StartSizeVar = 1,
			// 	.StartSpin = 0,
			// 	.StartSpinVar = 0,
			// 	.StartColorR = 0,
			// 	.StartColorVarR = 0,
			// 	.StartColorG = 1,
			// 	.StartColorVarG = 0,
			// 	.StartColorB = 1,
			// 	.StartColorVarB = 0,
			// 	.StartColorA = 1,
			// 	.StartColorVarA = 0,
			// 	.EndSize = 1,
			// 	.EndSizeVar = 1,
			// 	.EndSpin = 0,
			// 	.EndSpinVar = 0,
			// 	.EndColorR = 0,
			// 	.EndColorVarR = 0,
			// 	.EndColorG = 0.784314,
			// 	.EndColorVarG = 0,
			// 	.EndColorB = 1,
			// 	.EndColorVarB = 0,
			// 	.EndColorA = 1,
			// 	.EndColorVarA = 0,
			// 	.FadeInTime = 0.27,
			// 	.FadeInTimeVar = 0,
			// 	.FadeOutTime = 0.27,
			// 	.FadeOutTimeVar = 0,
			// 	.StartRadius = 0,
			// 	.StartRadiusVar = 0,
			// 	.EndRadius = 0,
			// 	.EndRadiusVar = 0,
			// 	.RotatePerSecond = 0,
			// 	.RotatePerSecondVar = 0,
			// 	.EmitterMode = 0,
			// 	.PositionType = 2,
			// 	.isBlendAdditive = true,
			// 	.startSpinEqualToEndSpin = false,
			// 	.rotationIsDir = false,
			// 	.dynamicRotationIsDir = false,
			// 	.customParticleIdx = 0,
			// 	.uniformColorMode = false,
			// 	.frictionPos = 0,
			// 	.frictionPosVar = 0.25,
			// 	.respawn = 0,
			// 	.respawnVar = 0,
			// 	.orderSensitive = false,
			// 	.startSizeEqualToEndSize = false,
			// 	.startRadiusEqualToEndRadius = false,
			// 	.startRGBVarSync = false,
			// 	.endRGBVarSync = false,
			// 	.frictionSize = 0,
			// 	.frictionSizeVar = 0,
			// 	.frictionRot = 0,
			// 	.frictionRotVar = 0,
			// 	.sFrame = "particle_00_001.png"
			// };
			cocos2d::ParticleStruct particleStruct{};
			particleStruct.TotalParticles = 30;
			particleStruct.Duration = -1;
			particleStruct.Life = 1.3;
			particleStruct.LifeVar = 0.2;
			particleStruct.EmissionRate = 20;
			particleStruct.Angle = 90;
			particleStruct.AngleVar = 0;
			particleStruct.Speed = 10;
			particleStruct.SpeedVar = 5;
			particleStruct.PosVarX = 20;
			particleStruct.PosVarY = 20;
			particleStruct.GravityX = 0;
			particleStruct.GravityY = 0;
			particleStruct.RadialAccel = 8;
			particleStruct.RadialAccelVar = 0;
			particleStruct.TangentialAccel = 0;
			particleStruct.TangentialAccelVar = 0;
			particleStruct.StartSize = 4;
			particleStruct.StartSizeVar = 1;
			particleStruct.StartSpin = 0;
			particleStruct.StartSpinVar = 0;
			particleStruct.StartColorR = 0;
			particleStruct.StartColorVarR = 0;
			particleStruct.StartColorG = 1;
			particleStruct.StartColorVarG = 0;
			particleStruct.StartColorB = 1;
			particleStruct.StartColorVarB = 0;
			particleStruct.StartColorA = 1;
			particleStruct.StartColorVarA = 0;
			particleStruct.EndSize = 1;
			particleStruct.EndSizeVar = 1;
			particleStruct.EndSpin = 0;
			particleStruct.EndSpinVar = 0;
			particleStruct.EndColorR = 0;
			particleStruct.EndColorVarR = 0;
			particleStruct.EndColorG = 0.784314;
			particleStruct.EndColorVarG = 0;
			particleStruct.EndColorB = 1;
			particleStruct.EndColorVarB = 0;
			particleStruct.EndColorA = 1;
			particleStruct.EndColorVarA = 0;
			particleStruct.FadeInTime = 0.27;
			particleStruct.FadeInTimeVar = 0;
			particleStruct.FadeOutTime = 0.27;
			particleStruct.FadeOutTimeVar = 0;
			particleStruct.StartRadius = 0;
			particleStruct.StartRadiusVar = 0;
			particleStruct.EndRadius = 0;
			particleStruct.EndRadiusVar = 0;
			particleStruct.RotatePerSecond = 0;
			particleStruct.RotatePerSecondVar = 0;
			particleStruct.EmitterMode = 0;
			particleStruct.PositionType = 2;
			particleStruct.isBlendAdditive = true;
			particleStruct.startSpinEqualToEndSpin = false;
			particleStruct.rotationIsDir = false;
			particleStruct.dynamicRotationIsDir = false;
			particleStruct.customParticleIdx = 0;
			particleStruct.uniformColorMode = false;
			particleStruct.frictionPos = 0;
			particleStruct.frictionPosVar = 0.25;
			particleStruct.respawn = 0;
			particleStruct.respawnVar = 0;
			particleStruct.orderSensitive = false;
			particleStruct.startSizeEqualToEndSize = false;
			particleStruct.startRadiusEqualToEndRadius = false;
			particleStruct.startRGBVarSync = false;
			particleStruct.endRGBVarSync = false;
			particleStruct.frictionSize = 0;
			particleStruct.frictionSizeVar = 0;
			particleStruct.frictionRot = 0;
			particleStruct.frictionRotVar = 0;
			particleStruct.sFrame = "particle_00_001.png";

			m_mythic_particles = GameToolbox::particleFromStruct(particleStruct, nullptr, false);

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
