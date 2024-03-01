#include "RLDifficultyNode.hpp"
#include "../utils.hpp"

RLDifficultyNode* RLDifficultyNode::create(GJDifficulty difficulty, bool featured, bool epic)
{
	auto ret = new RLDifficultyNode();

	if (ret && ret->init(difficulty, featured, epic))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLDifficultyNode::init(GJDifficulty difficulty, bool featured, bool epic)
{
	if (!CCNode::init()) return false;

	m_difficulty_info = { difficulty, featured, epic };
	m_color = { 255, 255, 255 };

	m_difficulty_sprite = CCSprite::createWithSpriteFrameName(
		rl::constants::difficulty_to_sprite.at(difficulty).data()
	);
	m_difficulty_sprite->setID("difficulty-sprite");
	this->addChild(m_difficulty_sprite);

	if (featured)
	{
		m_featured_sprite = CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");
		m_featured_sprite->setPosition(m_difficulty_sprite->getPosition());
		m_featured_sprite->setID("featured-sprite");
		this->addChild(m_featured_sprite, -1);
	}

	if (epic)
	{
		m_epic_sprite = CCSprite::createWithSpriteFrameName("GJ_epicCoin_001.png");
		m_epic_sprite->setPosition(m_difficulty_sprite->getPosition());
		m_epic_sprite->setID("epic-sprite");
		this->addChild(m_epic_sprite, -1);
	}

	this->setContentSize(m_difficulty_sprite->getContentSize());
	m_difficulty_sprite->setPosition(this->getContentSize() / 2);

	return true;
}

void RLDifficultyNode::setColor(const ccColor3B& color)
{
	m_difficulty_sprite->setColor(color);
	if (m_featured_sprite)
		m_featured_sprite->setColor(color);
	if (m_epic_sprite)
		m_epic_sprite->setColor(color);

	m_color = color;
}

void RLDifficultyNode::setDifficulty(GJDifficulty difficulty, bool featured, bool epic)
{
	if (m_difficulty_info == DifficultyInfo{ difficulty, featured, epic }) return;

	for (auto* node : CCArrayExt<CCSprite*>(this->getChildren()))
		node->removeFromParent();

	m_difficulty_sprite = CCSprite::createWithSpriteFrameName(
		rl::constants::difficulty_to_sprite.at(difficulty).data()
	);
	m_difficulty_sprite->setColor(m_color);
	m_difficulty_sprite->setID("difficulty-sprite");
	this->addChild(m_difficulty_sprite);

	if (featured)
	{
		m_featured_sprite = CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");
		m_featured_sprite->setColor(m_color);
		m_featured_sprite->setPosition(m_difficulty_sprite->getPosition());
		m_featured_sprite->setID("featured-sprite");
		this->addChild(m_featured_sprite, -1);
	}

	if (epic)
	{
		m_epic_sprite = CCSprite::createWithSpriteFrameName("GJ_epicCoin_001.png");
		m_epic_sprite->setColor(m_color);
		m_epic_sprite->setPosition(m_difficulty_sprite->getPosition());
		m_epic_sprite->setID("epic-sprite");
		this->addChild(m_epic_sprite, -1);
	}

	// man this codebase is getting worse by the minute
	// TODO: is there a better way :despair:
	if (this->getParent() && typeinfo_cast<CCMenuItemSpriteExtra*>(this->getParent()))
		m_difficulty_sprite->setPosition(this->getContentSize() / 2);

	m_difficulty_info = { difficulty, featured, epic };
}
