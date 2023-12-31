#include "CreatorLayer.hpp"
#include "../utils.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"

class RouletteButton final : public gd::FLAlertLayer, public CCTextFieldDelegate, public gd::FLAlertLayerProtocol
{
public:
	void onRouletteButton(CCObject*)
	{
		RouletteLayer::create()->show();

		if (!RouletteManager.rouletteResourcesFound)
		{
			FLAlertLayer::create(
				nullptr, "Resources", "OK", nullptr,
				"The <cg>roulette's resources</c> were not loaded.\n"
				"This may be due to an <cr>incorrect installation</c>.\n"
				"Please check the Github for more information."
			)->show();
			RouletteManager.rouletteResourcesFound = true;
		}

		if (RouletteManager.isJsonCorrupted)
		{
			FLAlertLayer::create(
				nullptr, "JSON file", "OK", nullptr,
				"The <cg>roulette's save file</c> was corrupted.\n"
				"As such, the options have been <cr>reset to their default values</c>."
			)->show();

			RouletteManager.isJsonCorrupted = false;
		}
	}
};

std::array<const char*, 6> rouletteSprites{
	"RL_blankBtn_001-hd.png",
	"RL_blankBtn_001-uhd.png",
	"RL_blankBtn_001.png",
	"RL_btn_001-hd.png",
	"RL_btn_001-uhd.png",
	"RL_btn_001.png"
};

bool __fastcall CreatorLayer::initHook(gd::CreatorLayer* self)
{
	namespace fs = std::filesystem;

	if (!CreatorLayer::init(self)) return false;

	auto menu = reinterpret_cast<CCMenu*>(self->getChildren()->objectAtIndex(1));

	fs::path resourcesPath = fs::current_path() / "Resources";
	int spritesFound = 0;

	for (auto& spriteName : rouletteSprites)
		if (fs::exists(resourcesPath / spriteName))
			spritesFound++;
		else
			break;

	RouletteManager.rouletteResourcesFound = spritesFound == rouletteSprites.size();
	
	gd::CCMenuItemSpriteExtra* rouletteButton{};
	CCPoint buttonPos = utils::isProcessLoaded("hackpro.dll", GetCurrentProcessId()) ? CCPoint{ -255.f, 0.f } : CCPoint{ -235.f, -110.f };

	if (RouletteManager.rouletteResourcesFound)
	{
		rouletteButton = gd::CCMenuItemSpriteExtra::create(
			CCSprite::create("RL_blankBtn_001.png"),
			self,
			menu_selector(RouletteButton::onRouletteButton)
		);
		auto rouletteSprite = CCSprite::create("RL_btn_001.png");
		float scale;
		switch (gd::GameManager::sharedState()->m_eQuality)
		{
		case TextureQuality::kTextureQualityLow:
			scale = .009f;
			break;
		case TextureQuality::kTextureQualityMedium:
			scale = .017f;
			break;
		case TextureQuality::kTextureQualityHigh:
			scale = .035f;
			break;
		}
		rouletteSprite->setScale(.035f);
		rouletteSprite->setPosition({ 22.5f, 23.5f });
		rouletteButton->addChild(rouletteSprite);
	}
	else
	{
		rouletteButton = gd::CCMenuItemSpriteExtra::create(
			gd::ButtonSprite::create("R", 10, true, "bigFont.fnt", "GJ_button_06.png", .0f, 1.f),
			self,
			menu_selector(RouletteButton::onRouletteButton)
		);
	}

	rouletteButton->setPosition(buttonPos);
	menu->addChild(rouletteButton);

	return true;
}
