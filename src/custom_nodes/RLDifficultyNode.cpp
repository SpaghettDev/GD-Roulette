#include "RLDifficultyNode.hpp"
#include "../utils.hpp"

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
	auto* ret = new RLDifficultyNode();

	if (ret && ret->init({ difficulty, RL_FEATURE_STATE::NONE }))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLDifficultyNode::init(const DifficultyInfo& di)
{
	if (!CCNodeRGBA::init()) return false;
	
	m_difficulty_info = di;

	this->setCascadeColorEnabled(true);
	this->setCascadeOpacityEnabled(true);


	m_difficulty_sprite = CCSprite::createWithSpriteFrameName(
		rl::constants::difficulty_to_sprite.at(m_difficulty_info.difficulty).data()
	);
	m_difficulty_sprite->setID("difficulty-sprite");
	this->addChild(m_difficulty_sprite);

	// TODO: fix this setting the position relative to the bottom left when not in a CCMenuItemSpriteExtra
	const CCPoint& targetContentSize = m_difficulty_sprite->getContentSize();
	this->setContentSize(targetContentSize);
	m_difficulty_sprite->setPosition(targetContentSize / 2);

	switch (m_difficulty_info.feature_state)
	{
		case RL_FEATURE_STATE::FEATURED:
		case RL_FEATURE_STATE::EPIC:
		case RL_FEATURE_STATE::LEGENDARY:
		case RL_FEATURE_STATE::MYTHIC:
			m_feature_sprite = CCSprite::createWithSpriteFrameName(
				rl::constants::feature_state_to_sprite.at(m_difficulty_info.feature_state).data()
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
			rl::constants::difficulty_to_sprite.at(di.difficulty).data()
		);
		m_difficulty_sprite->setID("difficulty-sprite");
		this->addChild(m_difficulty_sprite);
	}

	// man this codebase is getting worse by the minute
	// TODO: find a better way :despair:
	if (this->getParent() && typeinfo_cast<CCMenuItemSpriteExtra*>(this->getParent()))
		m_difficulty_sprite->setPosition(this->getContentSize() / 2);

	if (m_difficulty_info.feature_state != di.feature_state)
	{
		if (m_feature_sprite)
			m_feature_sprite->removeFromParent();

		switch (di.feature_state)
		{
			case RL_FEATURE_STATE::FEATURED:
			case RL_FEATURE_STATE::EPIC:
			case RL_FEATURE_STATE::LEGENDARY:
			case RL_FEATURE_STATE::MYTHIC:
				m_feature_sprite = CCSprite::createWithSpriteFrameName(
					rl::constants::feature_state_to_sprite.at(di.feature_state).data()
				);
			break;

			default:
				m_feature_sprite = nullptr;
			break;
		}

		if (m_feature_sprite)
		{
			m_feature_sprite->setID("feature-sprite");
			this->addChild(m_feature_sprite, -1);
		}
	}

	m_difficulty_info = di;
}

void RLDifficultyNode::setDifficulty(GJDifficulty difficulty)
{
	setDifficulty({ difficulty, RL_FEATURE_STATE::NONE });
}
