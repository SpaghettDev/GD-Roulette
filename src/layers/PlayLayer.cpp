#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include "../custom_nodes/RLProgressBar.hpp"
#include "RouletteManager.hpp"

using namespace geode::prelude;

class $modify(RLPlayLayer, PlayLayer)
{
	struct Fields
	{
		CCNode* m_container_node = nullptr;
		CCNode* m_progress_bar_container = nullptr;

		bool m_has_set_progress_color = false;
	};

	void resetLevel()
	{
		if (RouletteManager::get().isPlaying && !Mod::get()->getSettingValue<bool>("hide-goal-reached"))
		{
			containerNodeCleanup();
			progressBarContainerCleanup();
		}

		if (m_fields->m_has_set_progress_color)
		{
			m_fields->m_has_set_progress_color = false;
			this->m_percentageLabel->setColor({ 255, 255, 255 });
		}

		PlayLayer::resetLevel();
	}

	void updateProgressbar()
	{
		PlayLayer::updateProgressbar();

		const auto& rlm = RouletteManager::get();

		if (!this->m_percentageLabel || !rlm.isPlaying || m_fields->m_has_set_progress_color)
			return;

		if (std::floor(this->getCurrentPercent()) >= rlm.gameState.levelPercentageGoal)
		{
			this->m_percentageLabel->runAction(
				CCTintTo::create(.3f, 255, 255, 0)
			);
			m_fields->m_has_set_progress_color = true;
		}
	}

	void destroyPlayer(PlayerObject* player, GameObject* obj)
	{
		auto& rlm = RouletteManager::get();

		rlm.hasEnteredPlayLayer = true;

		if (obj == this->m_anticheatSpike)
			return PlayLayer::destroyPlayer(player, obj);

		const int percentage = std::floor(this->getCurrentPercent());
		const bool isValidAttempt = rlm.isPlaying &&
			this->m_level->m_levelID == rlm.gameState.levelID &&
			!this->m_isPracticeMode;
		const bool hasReachedGoal = isValidAttempt &&
			percentage >= rlm.gameState.levelPercentageGoal;

		if (isValidAttempt)
		{
			rlm.gameState.levelPercentage = percentage;

			rlm.gameState.levelAttempts++;
			rlm.gameState.totalAttempts++;
		}

		if (hasReachedGoal)
		{
			rlm.gameState.hasReachedGoal = true;
			rlm.gameState.levelPercentage = percentage;
			rlm.gameState.levelPercentageGoal = percentage + 1;
			rlm.gameState.numLevels++;

			// if the player has already reached this percentage, showNewBest will get called
			if (percentage <= this->m_level->m_normalPercent.value())
				showReachedGoal(percentage, 0, 0, 0);

			rlm.saveState();
		}
		else if (m_fields->m_has_set_progress_color)
		{
			m_fields->m_has_set_progress_color = false;
			this->m_percentageLabel->setColor({ 255, 255, 255 });
		}

		PlayLayer::destroyPlayer(player, obj);

		// prevent delayedResetLevel from actually running its body
		if (hasReachedGoal)
			this->m_inResetDelay = false;
	}

	void showNewBest(bool isNewReward, int orbs, int diamonds, bool demonKey, bool shouldNotSetDelegate, bool p5)
	{
		auto& rlm = RouletteManager::get();

		if (
			rlm.isPlaying &&
			this->m_level->m_levelID == rlm.gameState.levelID &&
			!this->m_isPracticeMode &&
			rlm.gameState.hasReachedGoal
		) {
			return showReachedGoal(std::floor(this->getCurrentPercent()), orbs, diamonds, demonKey);
		}

		PlayLayer::showNewBest(isNewReward, orbs, diamonds, demonKey, shouldNotSetDelegate, p5);
	}

	void levelComplete()
	{
		auto& rlm = RouletteManager::get();

		if (
			rlm.isPlaying &&
			this->m_level->m_levelID == rlm.gameState.levelID &&
			!this->m_isPracticeMode
		) {
			rlm.gameState.hasReachedGoal = true;
			rlm.gameState.levelPercentage = 100;
			rlm.gameState.levelPercentageGoal = 100;
			rlm.gameState.numLevels++;

			rlm.saveState();
		}

		PlayLayer::levelComplete();
	}

	void showCompleteText()
	{
		PlayLayer::showCompleteText();

		if (!RouletteManager::get().isPlaying || this->m_isPracticeMode || !RouletteManager::get().gameState.hasReachedGoal)
			return;

		auto levelCompleteSprite = this->getChildByType<CCSprite>(1);
		levelCompleteSprite->setVisible(false);

		auto goalReachedSprite = CCSprite::create("RL_goalReached_001.png"_spr);
		goalReachedSprite->setScale(levelCompleteSprite->getScale());
		goalReachedSprite->setPosition(levelCompleteSprite->getPosition());
		goalReachedSprite->setID("goal-reached-sprite"_spr);
		this->addChild(goalReachedSprite);

		if (GameManager::get()->getGameVariable("0095"))
			goalReachedSprite->setFlipX(true);

		goalReachedSprite->runAction(
			CCSequence::create(
				CCEaseElasticOut::create(
					CCScaleTo::create(.66f, 1.1f),
					.6f
				),
				CCDelayTime::create(.88f),
				CCEaseIn::create(
					CCScaleTo::create(.22f, .0f, .0f),
					2.f
				),
				CCCallFuncN::create(goalReachedSprite, callfuncN_selector(RLPlayLayer::nodeCleanup)),
				nullptr
			)
		);
	}


	void nodeCleanup(CCNode* node)
	{
		if (node)
			node->removeMeAndCleanup();
	}

	void containerNodeCleanup()
	{
		if (!m_fields->m_container_node) return;

		m_fields->m_container_node->removeMeAndCleanup();
		m_fields->m_container_node = nullptr;
	}
	void progressBarContainerCleanup()
	{
		if (!m_fields->m_progress_bar_container) return;

		m_fields->m_progress_bar_container->removeMeAndCleanup();
		m_fields->m_progress_bar_container = nullptr;
	}

	void showReachedGoal(int percentage, int orbs, int diamonds, bool demonKey)
	{
		const auto hideGoalReached = Mod::get()->getSettingValue<bool>("hide-goal-reached");

		CCSequence* containerSequence;
		auto winSize = CCDirector::sharedDirector()->getWinSize();

		auto containerNode = CCNode::create();
		m_fields->m_container_node = containerNode;
		containerNode->setID("container-node"_spr);
		containerNode->setPosition({ winSize.width * .5f, (winSize.height * .5f) + 20.f });
		containerNode->setScale(.01f);
		this->addChild(containerNode, 100);

		auto progressBarContainer = CCNode::create();
		m_fields->m_progress_bar_container = progressBarContainer;
		progressBarContainer->setID("progress-bar-container"_spr);
		progressBarContainer->setPosition({ winSize.width * .5f, -15.f });
		progressBarContainer->setScale(.5f);
		this->addChild(progressBarContainer, 100);


		auto goalReachedSprite = CCSprite::create("RL_goalReached_001.png"_spr);
		goalReachedSprite->setAnchorPoint({ .5f, .0f });
		containerNode->addChild(goalReachedSprite);

		auto progressBar = RLProgressBar::create(percentage, true);
		progressBar->setColor({ 255, 255, 0 });
		progressBar->setID("progress-bar");
		progressBarContainer->addChild(progressBar);

		auto levelAttempts = RouletteManager::get().gameState.levelAttempts;
		auto percentageLabel = CCLabelBMFont::create(
			fmt::format(
				"in {} Attempt{}",
				levelAttempts,
				levelAttempts < 2 ? "" : "s"
			).c_str(),
			"bigFont.fnt"
		);
		percentageLabel->setScale(.9f);
		percentageLabel->setAnchorPoint({ .5f, 1.f });
		percentageLabel->setPosition({ .0f, -4.f });
		containerNode->addChild(percentageLabel);

		if (orbs > 0)
		{
			auto orbsLabel = CCLabelBMFont::create(fmt::format("+{}", orbs).c_str(), "bigFont.fnt");
			orbsLabel->setScale(.6f);
			orbsLabel->setPosition({ -16.f, -45.f });
			containerNode->addChild(orbsLabel);

			auto orbsSprite = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
			orbsSprite->setPosition({ 11.f, -45.f });
			containerNode->addChild(orbsSprite);

			FMODAudioEngine::get()->playEffect("magicExplosion.ogg");

			if (!hideGoalReached)
			{
				CCSequence* orbsSequence = CCSequence::create(
					CCDelayTime::create(2.f),
					CCFadeOut::create(.4f),
					nullptr
				);

				orbsLabel->runAction(static_cast<CCAction*>(orbsSequence->copy()));
				orbsSprite->runAction(orbsSequence);
			}
		}

		if (hideGoalReached)
			containerSequence = CCSequence::create(
				CCEaseElasticOut::create(
					CCScaleTo::create(.4f, 1.f),
					.6f
				),
				CCDelayTime::create(.7f),
				CCEaseIn::create(
					CCScaleTo::create(.2f, .0f, .0f),
					2.f
				),
				nullptr
			);
		else
			containerSequence = CCSequence::create(
				CCEaseElasticOut::create(
					CCScaleTo::create(.4f, 1.f),
					.6f
				),
				nullptr
			);

		containerNode->runAction(containerSequence);


		if (orbs > 0)
		{
			auto currencyRewardLayer = CurrencyRewardLayer::create(
				orbs, 0, 0, diamonds,
				demonKey ? CurrencySpriteType::DemonKey : CurrencySpriteType::Icon,
				demonKey,
				CurrencySpriteType::Icon,
				0,
				this->m_player1->getParent()->convertToWorldSpace(this->m_player1->getPosition()),
				static_cast<CurrencyRewardType>(0),
				.0f,
				.9f
			);
			currencyRewardLayer->m_delegate = this;

			if (demonKey)
				FMODAudioEngine::get()->playEffect("secretKey.ogg");

			currencyRewardLayer->setPosition({ .0f, .0f });
			this->addChild(currencyRewardLayer, 101);
		}


		progressBarContainer->runAction(
			CCEaseBackInOut::create(
				CCScaleTo::create(.3f, 1.f)
			)
		);

		if (hideGoalReached)
			progressBarContainer->runAction(
				CCSequence::create(
					CCEaseBackInOut::create(
						CCMoveTo::create(.5f, { winSize.width * .5f, 30.f })
					),
					CCCallFunc::create(progressBar, callfunc_selector(RLProgressBar::playFillingAnimation)),
					CCDelayTime::create(2.f),
					CCEaseBackOut::create(
						CCMoveTo::create(.5f, { winSize.width * .5f, -15.f })
					),
					CCCallFunc::create(containerNode, callfunc_selector(RLPlayLayer::containerNodeCleanup)),
					CCCallFunc::create(progressBarContainer, callfunc_selector(RLPlayLayer::progressBarContainerCleanup)),
					nullptr
				)
			);
		else
			progressBarContainer->runAction(
				CCSequence::create(
					CCEaseBackInOut::create(
						CCMoveTo::create(.5f, { winSize.width * .5f, 30.f })
					),
					CCCallFunc::create(progressBar, callfunc_selector(RLProgressBar::playFillingAnimation)),
					nullptr
				)
			);
	}
};
