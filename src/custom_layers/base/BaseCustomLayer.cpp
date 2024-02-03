#include "BaseCustomLayer.hpp"
#include "../CustomDirector.hpp"

bool BaseCustomLayer::createBasics(CCPoint contentSize, SEL_MenuHandler onClose, float closeBtnScale, const ccColor4B& color)
{
	if (!CCLayerColor::initWithColor(color)) return false;

	alertSize = contentSize;

	CCDirector* director = CCDirector::sharedDirector();

	setTouchEnabled(true);
	setKeypadEnabled(true);

	m_mainLayer = CCLayer::create();
	this->addChild(m_mainLayer);

	m_mainLayer->setTouchPriority(director->getTouchDispatcher()->getTargetPrio());

	CCSize winSize = director->getWinSize();
	extension::CCScale9Sprite* bg = extension::CCScale9Sprite::create("GJ_square01.png", { .0f, .0f, 80.f, 80.f });
	bg->setContentSize(alertSize);
	bg->setPosition({ winSize.width / 2, winSize.height / 2 });
	m_mainLayer->addChild(bg, -1);

	m_buttonMenu = CCMenu::create();
	m_mainLayer->addChild(m_buttonMenu, 10);

	closeBtn = createButton("GJ_closeBtn_001.png", { -((alertSize.x) / 2) + 9.5f, (alertSize.y / 2) - 10 }, onClose, -1, closeBtnScale);

	return true;
}

void BaseCustomLayer::createTitle(std::string text, float separatorScale, float usernameScale)
{
	auto userName = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
	userName->setPosition({ 0, (alertSize.y / 2) - 22 });
	userName->setScale(usernameScale);
	m_buttonMenu->addChild(userName);

	auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
	separator->setPosition({ 0, (alertSize.y / 2) - 46 });
	separator->setScaleX(separatorScale);
	separator->setOpacity(100);
	m_buttonMenu->addChild(separator);
}

CCMenuItemSpriteExtra* BaseCustomLayer::createButton(const char* texture, CCPoint position, SEL_MenuHandler callback, int tag, float textureScale, float sizeMult)
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

cocos2d::CCScene* BaseCustomLayer::scene()
{
	return nullptr;
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
