#pragma once

#include "minhookcpp.h"
#include "Utility.h"
#include <mutex>
#include <functional>
#include <MinHook.h>

struct Hooks {
	static HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
};


class HookManager {
	friend Hooks;
private:
	HookManager() :
		hLoadLibraryA(LoadLibraryA, Hooks::LoadLibraryA, "LoadLibraryA") {}
public:
	static HookManager* initialize() {
		static std::once_flag fInit;
		std::call_once(fInit, std::mem_fn(&std::unique_ptr<HookManager>::reset), &instance, new HookManager());
		return instance.get();
	}

	static void destroy() {
		instance.reset();
	}

	void enable_all() {
		hLoadLibraryA.enable();
	}

private:
	static std::unique_ptr<HookManager> instance;

	struct MINHOOK {
		MINHOOK() { fmt::output_debug("{} {}", "MH_Initialize", (int)MH_Initialize()); }
		~MINHOOK() { fmt::output_debug("{} {}", "MH_Uninitialize", (int)MH_Uninitialize()); }
	} minHook;

	MinHookCpp<decltype(LoadLibraryA)> hLoadLibraryA;
};

inline HookManager* gHookManager = HookManager::initialize();
