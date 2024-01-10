#pragma once

#include <mutex>
#include <functional>
#include <dxgi.h>

#include <MinHook.h>

#include "minhookcpp.h"
#include "utils/Utility.h"
#include "Utils/Singleton.h"
#include "Logger.h"
#include "Pointers.h"

struct Hooks {
	static HMODULE WINAPI LoadLibraryA(LPCSTR lpLibFileName);

	static HRESULT STDMETHODCALLTYPE Present(IDXGISwapChain* ths, UINT SyncInterval, UINT Flags);
	static HRESULT STDMETHODCALLTYPE ResizeBuffers(
		IDXGISwapChain* ths,
		/* [in] */ UINT BufferCount,
		/* [in] */ UINT Width,
		/* [in] */ UINT Height,
		/* [in] */ DXGI_FORMAT NewFormat,
		/* [in] */ UINT SwapChainFlags
	);
};


class HookManager {
	friend Hooks;
	friend Singleton<HookManager>;
private:
	HookManager() try:
		LoadLibraryA(::LoadLibraryA, Hooks::LoadLibraryA, "LoadLibraryA"),
		Present(gPointers->fPresent, Hooks::Present, "Present"),
		ResizeBuffers(gPointers->fResizeBuffers, Hooks::ResizeBuffers, "ResizeBuffers")
	{} catch (const std::runtime_error& err) {
		SPDLOG_WARN(err.what());
	}
public:
	void enable_all() {
		MH_EnableHook(MH_ALL_HOOKS);
	}

	void disable_all() {
		MH_DisableHook(MH_ALL_HOOKS);
	}
private:
	struct MINHOOK {
		MINHOOK() { MH_Initialize(); }
		~MINHOOK() { MH_Uninitialize(); }
	} minHook;

	MinHookCpp<decltype(::LoadLibraryA)> LoadLibraryA;
	MinHookCpp<decltype(Hooks::Present)> Present;
	MinHookCpp<decltype(Hooks::ResizeBuffers)> ResizeBuffers;
};

inline HookManager* gHookManager{};
