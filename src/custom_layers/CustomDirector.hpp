#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class CustomDirector : public CCDirector
{
public:
	CCScene* getPreviousScene()
	{
		return static_cast<CCScene*>(m_pobScenesStack->objectAtIndex(m_pobScenesStack->count() - 2));
	}
};
