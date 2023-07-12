#include "CreatorLayer.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"

class ButtonsClass final : public gd::FLAlertLayer, public CCTextFieldDelegate, public gd::FLAlertLayerProtocol
{
public:
	void onRouletteButton(CCObject* sender)
	{
		RouletteLayer::create()->show();

		if (!RouletteManager.rouletteResourcesFound)
		{
			FLAlertLayer::create(
				nullptr, "Resources", "Ok", nullptr,
				"The <cg>roulette's resources</c> were not loaded.\nThis may be due to an <cr>incorrect installation</c>.\nPlease check the Github for more information."
			)->show();
			RouletteManager.rouletteResourcesFound = true;
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

	if (RouletteManager.rouletteResourcesFound)
	{
		auto rouletteButton = gd::CCMenuItemSpriteExtra::create(
			CCSprite::create("RL_blankBtn_001.png"),
			self,
			menu_selector(ButtonsClass::onRouletteButton)
		);
		rouletteButton->setPosition({ -235.f, -110.f });
		auto rouletteSprite = CCSprite::create("RL_btn_001.png");
		rouletteSprite->setScale(.035f);
		rouletteSprite->setPosition({ 22.5f, 23.5f });
		rouletteButton->addChild(rouletteSprite);
		menu->addChild(rouletteButton);
	}
	else
	{
		auto rouletteButton = gd::CCMenuItemSpriteExtra::create(
			gd::ButtonSprite::create("R", 10, true, "bigFont.fnt", "GJ_button_06.png", .0f, 1.f),
			self,
			menu_selector(ButtonsClass::onRouletteButton)
		);
		rouletteButton->setPosition({ -235.f, -110.f });
		menu->addChild(rouletteButton);
	}

	return true;
}
