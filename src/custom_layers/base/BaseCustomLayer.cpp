#include "BaseCustomLayer.hpp"

using namespace geode::prelude;

bool BaseCustomLayer::init(const char* cornerSpriteName)
{
	if (!CCLayer::init()) return false;

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	this->setTouchEnabled(true);
	this->setKeypadEnabled(true);

	m_backgroundMenu = CCMenu::create();
	m_backgroundMenu->setPosition({ .0f, .0f });
	m_backgroundMenu->setAnchorPoint({ .0f, .0f });
	m_backgroundMenu->setID("background-menu");
	this->addChild(m_backgroundMenu, -2);

	m_backMenu = CCMenu::create();
	m_backMenu->setPosition({ 24.f, winSize.height - 23.f });
	m_backMenu->setID("back-menu");
	this->addChild(m_backMenu);

	m_backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
		this,
		menu_selector(BaseCustomLayer::onClose)
	);
	m_backBtn->setSizeMult(1.2f);
	m_backBtn->setID("back-button");
	m_backMenu->addChild(m_backBtn);

	m_sideArtMenu = CCMenu::create();
	m_sideArtMenu->setPosition({ .0f, .0f });
	m_sideArtMenu->setAnchorPoint({ .0f, .0f });
	m_sideArtMenu->setID("side-art-menu");
	this->addChild(m_sideArtMenu, -1);

	for (auto [isTop, isRight] : std::initializer_list<std::pair<bool, bool>>{ { true, false }, { true, true }, { false, false }, { false, true } })
	{
		auto cornerSprite = CCSprite::create(cornerSpriteName);
		cornerSprite->setFlipX(isRight);
		cornerSprite->setFlipY(isTop);
		cornerSprite->setPosition({ isRight ? winSize.width : .0f, isTop ? winSize.height : .0f });
		cornerSprite->setAnchorPoint({ isRight ? 1.f : .0f, isTop ? 1.f : .0f });
		m_sideArtMenu->addChild(cornerSprite);
	}

	return true;
}

// overriden because clicking space crashes the game
void BaseCustomLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape)
		onClose(nullptr);
}

void BaseCustomLayer::keyBackClicked()
{
	onClose(nullptr);
}
