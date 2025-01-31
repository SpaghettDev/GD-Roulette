// progress bars are not my forté https://github.com/adafcaefc/Geome3Dash/blob/master/Geome3Dash/src/game/component/G3DProgressBar.cpp
#include "RLProgressBar.hpp"

RLProgressBar* RLProgressBar::create(float progress, bool willAnimate)
{
	auto* ret = new RLProgressBar();

	if (ret && ret->init(progress, willAnimate))
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

RLProgressBar* RLProgressBar::create()
{
	return create(.0f, false);
}

bool RLProgressBar::init(float progress, bool willAnimate)
{
	if (!CCSprite::initWithFile("GJ_progressBar_001.png")) return false;

	m_progress = progress;
	m_animationElapsed = .0f;

	this->updateDisplayedColor({ 0, 0, 0 });
	this->setOpacity(100);

	m_clipper = cocos2d::CCClippingNode::create();
	m_stencil = cocos2d::CCDrawNode::create();
	m_clipper->setPosition({ .0f, .0f });
	m_clipper->setContentSize(this->getContentSize());
	updateClipper();
	m_clipper->setStencil(m_stencil);
	this->addChild(m_clipper);

	m_filling = cocos2d::CCSprite::create("GJ_progressBar_001.png");
	m_filling->setScaleX(.98f);
	m_filling->setScaleY(.7f);
	m_filling->setColor({ 255, 255, 255 });
	m_filling->setPosition(this->getContentSize() / 2.f);
	m_filling->setVisible(!willAnimate);
	m_clipper->addChild(m_filling);

	m_label = cocos2d::CCLabelBMFont::create(
		fmt::format("{}%",
			willAnimate ? 0 : static_cast<int>(std::round(m_progress))
		).c_str(),
		"bigFont.fnt"
	);
	m_label->setPosition(this->getContentSize() / 2.f);
	m_label->setScale(.5f);
	this->addChild(m_label);

	return true;
}

bool RLProgressBar::init()
{
	return init(.0f, false);
}

void RLProgressBar::updateClipper(float progress)
{
	progress = progress == -1.f ? m_progress : progress;

	m_stencil->clear();
	m_stencil->drawRect(
		{ 2.f, .0f, },
		{ ((this->getContentSize().width - 4.f) / 100.f * progress) + 2.f, this->getContentSize().height },
		{ 1.f, 1.f, 1.f, 1.f },
		0,
		{ 1.f, 1.f, 1.f, 1.f }
	);
}

void RLProgressBar::setColor(const cocos2d::ccColor3B& color)
{
	m_filling->setColor(color);
}

void RLProgressBar::setProgress(float progress)
{
	m_progress = progress;
	updateClipper();

	m_label->setString(
		fmt::format(
			"{}%",
			static_cast<int>(std::round(progress))
		).c_str()
	);
}

void RLProgressBar::update(float dt)
{
	if (m_animationElapsed >= m_animationDuration)
		return this->unschedule(schedule_selector(RLProgressBar::update));

	dt *= calcAnimationFactorAtTime(m_animationElapsed);

	if (m_isAnimationFirstTick)
	{
		m_animationElapsed = .0f;
		m_isAnimationFirstTick = false;
	}
	else
		m_animationElapsed += dt;

	float time = std::max(
		.0f,
		std::min(
			1.f,
			m_animationElapsed / std::max(m_animationDuration, std::numeric_limits<float>::epsilon())
		)
	);

	updateClipper(m_progress * time);
	m_label->setString(fmt::format("{}%", static_cast<int>(std::round(m_progress * time))).c_str());
}

void RLProgressBar::playFillingAnimation()
{
	updateClipper(.0f);
	m_label->setString("0%");

	m_filling->setVisible(true);

	m_animationDuration = calcAnimationDuration(m_progress);
	m_animationElapsed = .0f;
	m_isAnimationFirstTick = true;

	this->schedule(schedule_selector(RLProgressBar::update));
}
