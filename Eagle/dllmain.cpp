#include <Windows.h>

#include <thread>

#include "common.h"
#include "Global.h"
#include "Utils/Singleton.h"
#include "Hooking/HookManager.h"
#include "Logger.h"
#include "Pointers.h"
#include "render/Renderer.h"

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		global.m_thisModule = hModule; // TODO

		try {
			std::thread([]() {
				try {
					gLogger = Singleton<Logger>::initialize();
					gPointers = Singleton<Pointers>::initialize();
					gRenderer = Singleton<Renderer>::initialize();
					gHookManager = Singleton<HookManager>::initialize();
					global.m_running = true;
				} catch (std::runtime_error& err) {
					MessageBoxA(
						NULL,
						fmt::format("Launch Error\n{}", err.what()).c_str(),
						PROJECTNAME, MB_ICONWARNING);
				}
			}).detach();
		} catch (const std::exception& ex) {
			MessageBoxA(NULL, ex.what(), PROJECTNAME, MB_ICONERROR);
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
