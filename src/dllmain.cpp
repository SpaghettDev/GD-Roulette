#include "pch.hpp"

#include "layers/CreatorLayer.hpp"
#include "layers/LevelInfoLayer.hpp"
#include "layers/PlayLayer.hpp"

namespace common
{
	HMODULE hmodule;
	HANDLE mainThread;
	DWORD threadID;
}

BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace common;

	hmodule = hmod;

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hmod);

		mainThread = CreateThread(nullptr, 0, [](PVOID) -> DWORD {
			if (MH_Initialize() == MH_STATUS::MH_OK)
			{
				MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x4DE40), CreatorLayer::initHook,
					reinterpret_cast<void**>(&CreatorLayer::init));

				MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x01FB780), PlayLayer::initHook,
					reinterpret_cast<void**>(&PlayLayer::init));
				MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x2029C0), PlayLayer::updateHook,
					reinterpret_cast<void**>(&PlayLayer::update));
				MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x20A1A0), PlayLayer::destroyPlayerHook,
					reinterpret_cast<void**>(&PlayLayer::destroyPlayer));
				MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x20BF00), PlayLayer::resetLevelHook,
					reinterpret_cast<void**>(&PlayLayer::resetLevel));
				MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x1FD3D0), PlayLayer::levelCompleteHook,
					reinterpret_cast<void**>(&PlayLayer::levelComplete));

				MH_CreateHook(reinterpret_cast<void*>(gd::base + 0x17C110), LevelInfoLayer::onBackHook,
					reinterpret_cast<void**>(&LevelInfoLayer::onBack));


				MH_EnableHook(MH_ALL_HOOKS);
			}
			else
			{
				CloseHandle(mainThread);
				FreeLibraryAndExitThread(hmodule, 0);
			}

			return 1;
		}, nullptr, 0, &threadID);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return true;
}

