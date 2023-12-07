#include "CustomLayer.hpp"

bool CustomLayer::createBasics(CCPoint contentSize, SEL_MenuHandler onClose, float closeBtnScale, const ccColor4B& color)
{
	if (!CCLayerColor::initWithColor(color)) return false;

	alertSize = contentSize;

	CCDirector* director = CCDirector::sharedDirector();
	director->getTouchDispatcher()->incrementForcePrio(2);

	setTouchEnabled(true);
	setKeypadEnabled(true);

	m_pLayer = CCLayer::create();

	this->addChild(m_pLayer);

	CCSize winSize = director->getWinSize();
	extension::CCScale9Sprite* bg = extension::CCScale9Sprite::create("GJ_square01.png", { .0f, .0f, 80.0f, 80.0f });
	bg->setContentSize(alertSize);
	m_pLayer->addChild(bg, -1);
	bg->setPosition({ winSize.width / 2, winSize.height / 2 });

	m_pButtonMenu = CCMenu::create();
	m_pLayer->addChild(m_pButtonMenu, 10);

	closeBtn = createButton("GJ_closeBtn_001.png", { -((alertSize.x) / 2) + 9.5f, (alertSize.y / 2) - 10 }, onClose, -1, closeBtnScale);

	return true;
}

void CustomLayer::createTitle(std::string text, float separatorScale, float usernameScale)
{
	auto userName = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
	userName->setPosition({ 0, (alertSize.y / 2) - 22 });
	userName->setScale(usernameScale);
	m_pButtonMenu->addChild(userName);

	auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
	separator->setPosition({ 0, (alertSize.y / 2) - 46 });
	separator->setScaleX(separatorScale);
	separator->setOpacity(100);
	m_pButtonMenu->addChild(separator);
}

gd::CCMenuItemSpriteExtra* CustomLayer::createButton(const char* texture, CCPoint position, SEL_MenuHandler callback, int tag, float textureScale, float sizeMult)
{
	auto buttonSprite = CCSprite::createWithSpriteFrameName(texture);
	buttonSprite->setScale(textureScale);
	auto button = gd::CCMenuItemSpriteExtra::create(
		buttonSprite,
		this,
		callback
	);
	button->setPosition(position);
	button->setSizeMult(sizeMult);
	if (tag != -1)
		button->setTag(tag);
	m_pButtonMenu->addChild(button);

	return button;
}

// overriden because clicking space crashes the game
void CustomLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape)
		onClose(nullptr);
}

void CustomLayer::keyBackClicked()
{
	onClose(nullptr);
}
