#pragma once
#include <string>
#include <random>
#include <matjson.hpp>
#include "custom_layers/base/BaseCustomLayer.hpp"

namespace roulette::utils
{
	inline int randomInt(int min, int max)
	{
		std::random_device device;
		std::mt19937 generator(device());
		std::uniform_int_distribution<int> distribution(min, max);

		return distribution(generator);
	}

	template<typename T>
	inline std::ptrdiff_t getIndexOf(const std::vector<matjson::Value>& vec, T to_find)
	{
		auto it = std::find_if(vec.cbegin(), vec.cend(), [&](const matjson::Value arr) {
			return arr.as<T>() == to_find;
		});

		return it != vec.cend() ? (it - vec.cbegin()) : -1;
	}

	inline CCLabelBMFont* createTextLabel(const std::string& text, const CCPoint& position, const float scale, CCNode* menu, const char* font = "bigFont.fnt")
	{
		CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
		bmFont->setPosition(position);
		bmFont->setScale(scale);
		menu->addChild(bmFont);

		return bmFont;
	}

	inline CCMenuItemSpriteExtra* createButton(BaseCustomLayer* self, const char* texture, CCPoint position, SEL_MenuHandler callback, int tag = -1, float textureScale = 1.f, float sizeMult = 1.2f)
	{
		auto buttonSprite = CCSprite::createWithSpriteFrameName(texture);
		buttonSprite->setScale(textureScale);
		auto button = CCMenuItemSpriteExtra::create(
			buttonSprite,
			self,
			callback
		);
		button->setPosition(position);
		button->setSizeMult(sizeMult);
		if (tag != -1)
			button->setTag(tag);
		self->addChild(button);

		return button;
	}
}
