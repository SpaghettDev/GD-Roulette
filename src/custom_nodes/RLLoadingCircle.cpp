// not taken from https://github.com/acaruso-xx/slope-geode/blob/main/src/plate/LoadingCircle.cpp
#include "RLLoadingCircle.hpp"

RLLoadingCircle* RLLoadingCircle::create()
{
	auto ret = new RLLoadingCircle();

	if (ret && ret->init())
		ret->autorelease();
	else
	{
		delete ret;
		ret = nullptr;
	}

	return ret;
}

bool RLLoadingCircle::init()
{
	if (!this->initWithFile("loadingCircle.png")) return false;

	this->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
	this->setOpacity(0);
	this->setZOrder(105);

	return true;
}

void RLLoadingCircle::positionCenter()
{
	this->setPosition(cocos2d::CCDirector::sharedDirector()->getWinSize() / 2);
}

void RLLoadingCircle::fadeIn()
{
	auto* fadeInAction = cocos2d::CCFadeTo::create(.4f, 200);
	fadeInAction->setTag(ACTION_TAG::FADE_IN);

	this->runAction(fadeInAction);
}

void RLLoadingCircle::fadeOut()
{
	this->stopActionByTag(ACTION_TAG::FADE_IN);
	this->stopActionByTag(ACTION_TAG::ROTATE);

	auto* fadeOutAction = cocos2d::CCFadeTo::create(.4f, 0);
	fadeOutAction->setTag(ACTION_TAG::FADE_OUT);

	this->runAction(fadeOutAction);
}

void RLLoadingCircle::startRotate()
{
	auto* rotateAction = cocos2d::CCRepeatForever::create(
		cocos2d::CCRotateBy::create(1.f, 360.f)
	);
	rotateAction->setTag(ACTION_TAG::ROTATE);

	this->runAction(rotateAction);
}

void RLLoadingCircle::stopRotate()
{
	this->stopActionByTag(ACTION_TAG::ROTATE);
}

void RLLoadingCircle::show()
{
	fadeIn();
	startRotate();
}

void RLLoadingCircle::stopAndHide()
{
	stopRotate();
	fadeOut();
}
