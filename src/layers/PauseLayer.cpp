#include "PauseLayer.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"
#include "../roulette/layers/RouletteLayer.hpp"

namespace PauseLayer
{
	gd::PauseLayer* __fastcall createHook(bool p0, void*)
	{
		auto self = create(p0);

		if (RouletteManager.isPlayingRoulette)
		{
			CCNode* normalPercentageNode = nullptr;
			CCPoint normalPercentagePos = { 284.5f, 240.f };
			float goalOffset = 24.f;

			if (gd::PlayLayer::get()->m_level->normalPercent < 10)
				goalOffset = 28.f;
			else if (gd::PlayLayer::get()->m_level->normalPercent <= 100)
				goalOffset = 40.f;

			CCObject* pauseLayerObject;
			CCARRAY_FOREACH(self->getChildren(), pauseLayerObject)
			{
				CCNode* pauseLayerNode = reinterpret_cast<CCNode*>(pauseLayerObject);
				if (pauseLayerNode->getPositionX() == 284.5f && pauseLayerNode->getPositionY() == 240.f)
				{
					normalPercentageNode = pauseLayerNode;
					break;
				}
			};

			if (normalPercentageNode != nullptr)
				normalPercentagePos = normalPercentageNode->getPosition();

			auto goalPercentage = CCLabelBMFont::create(CCString::createWithFormat("(%d%%)", static_cast<int>(RouletteManager.levelPercentageGoal))->getCString(), "bigFont.fnt");
			goalPercentage->setPosition({ normalPercentagePos.x + goalOffset, normalPercentagePos.y });
			goalPercentage->setScale(.5f);
			goalPercentage->setColor({ 125, 125, 125 });
			goalPercentage->setZOrder(4);
			self->addChild(goalPercentage);
		}

		return self;
	}
}
