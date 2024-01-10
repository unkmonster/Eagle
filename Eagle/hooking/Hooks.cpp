#include "HookManager.h"
#include "Logger.h"

HMODULE WINAPI Hooks::LoadLibraryA(LPCSTR lpLibFileName) {
	auto result = gHookManager->hLoadLibraryA.get_origin()(lpLibFileName);
	SPDLOG_DEBUG("Loaded {} - 0x{:X}", lpLibFileName, (uintptr_t)result);
	return result;
}

