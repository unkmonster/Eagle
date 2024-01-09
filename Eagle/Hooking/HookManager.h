#pragma once

#include <mutex>
#include <functional>

#include <MinHook.h>

#include "minhookcpp.h"
#include "Utility.h"
#include "Utils/Singleton.h"
#include "Logger.h"

struct Hooks {
	static HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
};


class HookManager {
	friend Hooks;
	friend Singleton<HookManager>;
private:
	HookManager() :
		hLoadLibraryA(LoadLibraryA, Hooks::LoadLibraryA, "LoadLibraryA") {}
public:
	void enable_all() {
		hLoadLibraryA.enable();
	}

private:
	struct MINHOOK {
		MINHOOK() { MH_Initialize(); }
		~MINHOOK() { MH_Uninitialize(); }
	} minHook;

	MinHookCpp<decltype(LoadLibraryA)> hLoadLibraryA;
};

inline HookManager* gHookManager{};
