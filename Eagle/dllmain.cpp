#include <Windows.h>

#include <thread>
#include <chrono>

#include <MinHook.h>

#include "Global.h"
#include "Utility.h"
#include "Utils/Singleton.h"
#include "Hooking/HookManager.h"
#include "Logger.h"

using namespace std::chrono_literals;

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		global.dllModule = hModule;

		auto hThread = CreateThread(nullptr, NULL, [](LPVOID)->DWORD {
			gLogger = Singleton<Logger>::initialize();
			gHookManager = Singleton<HookManager>::initialize();
			gHookManager->enable_all();


			while (!(GetAsyncKeyState(VK_END) & 0x1))
				std::this_thread::sleep_for(1ms);

			FreeLibraryAndExitThread(global.dllModule, 0);
		}, nullptr, 0, nullptr);
		return TRUE;
	}
	return FALSE;
}