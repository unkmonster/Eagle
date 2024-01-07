#include "Global.h"
#include "Utility.h"
#include <Windows.h>
#include <thread>
#include <chrono>
#include <MinHook.h>
#include "Hooking/HookManager.h"

using namespace std::chrono_literals;

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		global.dllModule = hModule;

		auto hThread = CreateThread(nullptr, NULL, [](LPVOID)->DWORD {
			gHookManager->enable_all();

			while (!(GetAsyncKeyState(VK_END) & 0x1))
				std::this_thread::sleep_for(1ms);

			FreeLibraryAndExitThread(global.dllModule, 0);
		}, nullptr, 0, nullptr);
		return TRUE;
	}
	return FALSE;
}
