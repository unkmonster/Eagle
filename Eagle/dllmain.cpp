#include <Windows.h>

#include <thread>
#include <chrono>
#include <future>

#include <MinHook.h>

#include "Global.h"
#include "utils/Utility.h"
#include "Utils/Singleton.h"
#include "Hooking/HookManager.h"
#include "Logger.h"
#include "Pointers.h"
#include <render/Renderer.h>

using namespace std::chrono_literals;

DWORD Launcher(LPVOID param) {
	try {
		gLogger = Singleton<Logger>::initialize();
		gPointers = Singleton<Pointers>::initialize();
		gHookManager = Singleton<HookManager>::initialize();
		gRenderer = Singleton<Renderer>::initialize();
		
		while (!(GetAsyncKeyState(VK_END) & 0x1))
			std::this_thread::sleep_for(1ms);
	} catch (std::runtime_error& err) {
		MessageBoxA(NULL, err.what(), __func__, MB_ICONWARNING);
	}
	
	FreeLibraryAndExitThread(global.dllModule, 0);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		global.dllModule = hModule; // TODO

		if (CreateThread(nullptr, NULL, Launcher, nullptr, 0, nullptr) == NULL) {
			MessageBoxA(NULL, "Failed to create launch thread!", NULL, MB_ICONERROR);
			return FALSE;
		}
	} else if (reason == DLL_PROCESS_DETACH) {
		Singleton<HookManager>::destroy();
		Singleton<Renderer>::destroy();
		Singleton<Pointers>::destroy();
		Singleton<Logger>::destroy();
	}
	return TRUE;
}
