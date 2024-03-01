#include "RLDemonSelectLayer.hpp"

RLDemonSelectLayer* RLDemonSelectLayer::create(const RLDemonSelectLayerInfo& dsli)
{
	auto ret = new RLDemonSelectLayer();

	if (ret && ret->init(dsli))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLDemonSelectLayer::init(const RLDemonSelectLayerInfo& dsli)
{
	m_dsli = dsli;
	if (!this->createBasics({ 380.f, 180.f }, menu_selector(RLDemonSelectLayer::onClose), 1.f, { 0, 0, 0, 150 })) return false;

	{ // modify BaseCustomAlertLayer
		closeBtn->setVisible(false);
	}

	auto title = CCLabelBMFont::create("Demon Filter", "bigFont.fnt");
	title->setPosition({ .0f, 70.f });
	m_buttonMenu->addChild(title);


	// yeah...
	for (const auto& e : std::vector<std::pair<int, int>>{ { 0, 7 }, { 1, 8 }, { 2, 6 }, { 3, 9 }, { 4, 10 } })
	{
		auto demonSprite = CCSprite::createWithSpriteFrameName(fmt::format("difficulty_{0:0{1}}_btn2_001.png", e.second, 2).c_str());
		demonSprite->setScale(1.2f);
		auto demonButton = CCMenuItemSpriteExtra::create(
			demonSprite,
			this,
			menu_selector(RLDemonSelectLayer::onDemonButton)
		);
		if (m_dsli.selectedDifficulty != static_cast<GJDifficulty>(e.second))
			demonButton->setColor({ 125, 125, 125 });
		demonButton->setPosition({ -125.f + e.first * 60.f, 7.f });
		demonButton->setTag(e.second);
		m_buttonMenu->addChild(demonButton);
	}


	auto okButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.f),
		this,
		menu_selector(RLDemonSelectLayer::onClose)
	);
	okButton->setPosition({ .0f, -65.f });
	m_buttonMenu->addChild(okButton);


	return true;
}

void RLDemonSelectLayer::onClose(CCObject*)
{
	if (m_dsli.onOk)
		m_dsli.onOk(this);
	setKeypadEnabled(false);
	removeFromParentAndCleanup(true);
}

void RLDemonSelectLayer::onDemonButton(CCObject* sender)
{
	m_dsli.onSelectDifficulty(static_cast<GJDifficulty>(sender->getTag()), m_dsli.selectedDifficulty);
	m_dsli.selectedDifficulty = static_cast<GJDifficulty>(sender->getTag());

	for (auto* node : CCArrayExt<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()))
	{
		if (node->getTag() == sender->getTag())
			node->setColor({ 255, 255, 255 });
		else
			node->setColor({ 125, 125, 125 });
	}
}

void RLDemonSelectLayer::keyDown(enumKeyCodes key)
{
	if (key == enumKeyCodes::KEY_Escape)
		onClose(nullptr);
}