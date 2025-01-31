#include "BaseCustomAlertLayer.hpp"

using namespace geode::prelude;

bool BaseCustomAlertLayer::createBasics(const BaseCustomAlertData& alertData)
{
	if (!CCLayerColor::initWithColor(alertData.color)) return false;

	alertSize = alertData.contentSize;

	CCDirector* director = CCDirector::sharedDirector();

	this->setTouchEnabled(true);
	this->setKeypadEnabled(true);

	m_mainLayer = CCLayer::create();
	this->addChild(m_mainLayer);

	if (alertData.priority == std::numeric_limits<int>::max())
		m_mainLayer->setTouchPriority(director->getTouchDispatcher()->getTargetPrio());
	else
		m_mainLayer->setTouchPriority(alertData.priority);
	// this is what FLAlertLayer::incrementForcePrio does
	director->getTouchDispatcher()->registerForcePrio(this, 2);

	CCSize winSize = director->getWinSize();
	bg = cocos2d::extension::CCScale9Sprite::create(alertData.bgTextureName, alertData.bgRect);
	bg->setContentSize(alertSize);
	bg->setPosition({ winSize.width / 2.f, winSize.height / 2.f });
	m_mainLayer->addChild(bg, -1);

	m_buttonMenu = CCMenu::create();
	m_buttonMenu->setID("button-menu");
	m_mainLayer->addChild(m_buttonMenu, 10);

	closeBtn = createButton("GJ_closeBtn_001.png", { -((alertSize.x) / 2.f) + 9.5f, (alertSize.y / 2.f) - 10.f }, alertData.onClose, -1, alertData.closeBtnScale);

	return true;
}

void BaseCustomAlertLayer::createTitle(std::string text, float separatorScale, float usernameScale)
{
	auto userName = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
	userName->setPosition({ .0f, (alertSize.y / 2.f) - 25.f });
	userName->setScale(usernameScale);
	m_buttonMenu->addChild(userName);

	auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
	separator->setPosition({ .0f, (alertSize.y / 2.f) - 46.f });
	separator->setScaleX(separatorScale);
	separator->setOpacity(100);
	m_buttonMenu->addChild(separator);
}

CCMenuItemSpriteExtra* BaseCustomAlertLayer::createButton(const char* texture, CCPoint position, SEL_MenuHandler callback, int tag, float textureScale, float sizeMult)
{
	auto buttonSprite = CCSprite::createWithSpriteFrameName(texture);
	buttonSprite->setScale(textureScale);
	auto button = CCMenuItemSpriteExtra::create(
		buttonSprite,
		this,
		callback
	);
	button->setPosition(position);
	button->setSizeMult(sizeMult);
	if (tag != -1)
		button->setTag(tag);
	m_buttonMenu->addChild(button);

	return button;
}

// overriden because clicking space crashes the game
void BaseCustomAlertLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape)
		onClose(nullptr);
}

void BaseCustomAlertLayer::keyBackClicked()
{
	onClose(nullptr);
}
