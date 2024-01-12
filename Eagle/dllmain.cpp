#include <Windows.h>

#include <thread>
#include <chrono>
#include <future>

#include <MinHook.h>

#include "common.h"
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
		gRenderer = Singleton<Renderer>::initialize();
		gHookManager = Singleton<HookManager>::initialize();
		global.m_running = true;
		return 0;
		/*while (global.m_running)
			std::this_thread::sleep_for(1ms);*/
	} catch (std::runtime_error& err) {
		MessageBoxA(
			NULL, 
			fmt::format("Launch Error\n{}", err.what()).c_str(), 
			PROJECTNAME, MB_ICONWARNING);
		return 1;
	}
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		global.m_thisModule = hModule; // TODO

		if (CreateThread(nullptr, NULL, Launcher, nullptr, 0, nullptr) == NULL) {
			MessageBoxA(NULL, "Failed to create launch thread!", PROJECTNAME, MB_ICONERROR);
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
