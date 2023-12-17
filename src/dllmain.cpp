#include "pch.hpp"

#include "layers/CreatorLayer.hpp"
#include "layers/LevelInfoLayer.hpp"
#include "layers/PlayLayer.hpp"
#include "layers/PauseLayer.hpp"
#include "json_manager/JsonManager.hpp"

#include "utils.hpp"


BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace common;

	hmodule = hmod;

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hmod);

		mainThread = CreateThread(nullptr, 0,
		[](PVOID) -> DWORD {
#ifdef DEV_CONSOLE
			if (AllocConsole())
			{
				freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
				SetConsoleTitleW(L"GD-Roulette");
				SetConsoleCP(CP_UTF8);
				SetConsoleOutputCP(CP_UTF8);
			}
#endif // DEV_CONSOLE

			MH_Initialize();
			JsonManager();

			Sleep(utils::randomInt(250, 1000));

			
			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x4DE40), CreatorLayer::initHook,
				reinterpret_cast<void**>(&CreatorLayer::init)
			);

			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x01FB780), PlayLayer::initHook,
				reinterpret_cast<void**>(&PlayLayer::init)
			);
			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x2029C0), PlayLayer::updateHook,
				reinterpret_cast<void**>(&PlayLayer::update)
			);
			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x20A1A0), PlayLayer::destroyPlayerHook,
				reinterpret_cast<void**>(&PlayLayer::destroyPlayer)
			);
			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x20BF00), PlayLayer::resetLevelHook,
				reinterpret_cast<void**>(&PlayLayer::resetLevel)
			);
			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x1FD3D0), PlayLayer::levelCompleteHook,
				reinterpret_cast<void**>(&PlayLayer::levelComplete)
			);

			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x175DF0), LevelInfoLayer::initHook,
				reinterpret_cast<void**>(&LevelInfoLayer::init)
			);
			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x17C110), LevelInfoLayer::onBackHook,
				reinterpret_cast<void**>(&LevelInfoLayer::onBack)
			);

			MH_CreateHook(
				reinterpret_cast<void*>(gd::base + 0x1E4570), PauseLayer::createHook,
				reinterpret_cast<void**>(&PauseLayer::create)
			);


			MH_EnableHook(MH_ALL_HOOKS);

			return 0;
		}, nullptr, 0, &threadID);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return true;
}

