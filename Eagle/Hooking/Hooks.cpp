#include "HookManager.h"

HMODULE WINAPI Hooks::LoadLibraryA(LPCSTR lpLibFileName) {
	auto result = gHookManager->hLoadLibraryA.get_origin()(lpLibFileName);
	fmt::output_debug("Loaded {} - 0x{:X}", lpLibFileName, (uintptr_t)result);
	return result;
}

