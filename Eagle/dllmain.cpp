#include <Windows.h>
#include <thread>
#include <filesystem>

#include "bf-1/Frosbite.h"
#include "bf-1/Obfuscationmgr.h"

#include "common.h"
#include "Global.h"
#include "Utils/Singleton.h"
#include "Hooking/HookManager.h"
#include "Logger.h"
#include "Pointers.h"
#include "render/Renderer.h"
#include "FileManager.h"
#include "feature/PlayerManager.h"

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		global.m_thisModule = hModule; // TODO

		try {
			std::thread([]() {
				try {
					gLogger = Singleton<Logger>::initialize();

					char buffer[MAX_PATH]{};
					GetEnvironmentVariableA("appdata", buffer, MAX_PATH);
					gFileManager = Singleton<CFileManager>::initialize((std::filesystem::path(buffer) / "Eagle"));
					if (std::filesystem::exists(gFileManager->m_setting_bin)) {
						CFileManager::load(gFileManager->m_setting_bin, &global.m_setting);
						SPDLOG_INFO("Loaded Setting from disk");
					}
						
					gPointers = Singleton<Pointers>::initialize();
					gRenderer = Singleton<Renderer>::initialize();
					gHookManager = Singleton<HookManager>::initialize();
					gPlayerManager = Singleton<PlayerManager>::initialize();
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
		CFileManager::dump(gFileManager->m_setting_bin, &global.m_setting);
		Singleton<Logger>::destroy();
	}
	return TRUE;
}
