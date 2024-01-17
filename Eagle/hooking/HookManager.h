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
	static HRESULT STDMETHODCALLTYPE Present(IDXGISwapChain* ths, UINT SyncInterval, UINT Flags);
	static HRESULT STDMETHODCALLTYPE ResizeBuffers(
		IDXGISwapChain* ths,
		/* [in] */ UINT BufferCount,
		/* [in] */ UINT Width,
		/* [in] */ UINT Height,
		/* [in] */ DXGI_FORMAT NewFormat,
		/* [in] */ UINT SwapChainFlags
	);

	static LRESULT CALLBACK WNDPROC(HWND, UINT, WPARAM, LPARAM);
	static BOOL WINAPI SetCursorPos(int X, int Y);
};


class HookManager {
	friend Hooks;
	friend Singleton<HookManager>;
private:
	HookManager(bool enable_now = true) try:
		Present(gPointers->fPresent, Hooks::Present, "Present"),
		ResizeBuffers(gPointers->fResizeBuffers, Hooks::ResizeBuffers, "ResizeBuffers")
	{
		if (enable_now)
			enable_all();
	} catch (const std::runtime_error& err) {
		SPDLOG_WARN(err.what());
	}
public:
	~HookManager() {
		disable_all();
	}

	void enable_all() {
		MH_EnableHook(MH_ALL_HOOKS);
		SetWindowLongPtrA(gPointers->hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Hooks::WNDPROC));
	}

	void disable_all() {
		SetWindowLongPtrA(gPointers->hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(gPointers->oWndproc));
		MH_DisableHook(MH_ALL_HOOKS);
	}
private:
	struct MINHOOK {
		MINHOOK() { MH_Initialize(); }
		~MINHOOK() { MH_Uninitialize(); }
	} minHook;

	MinHookCpp<decltype(Hooks::Present)> Present;
	MinHookCpp<decltype(Hooks::ResizeBuffers)> ResizeBuffers;
	MinHookCpp<decltype(::SetCursorPos)> SetCursorPos{::SetCursorPos, Hooks::SetCursorPos, "SetCursorPos"};
};

inline HookManager* gHookManager{};
