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

	// Anti-Ss
	static BOOL WINAPI SetWindowDisplayAffinity(HWND hWnd, DWORD dwAffinity);
	static BOOL GetWindowDisplayAffinity(HWND hWnd, DWORD *pdwAffinity);
};


class HookManager {
	friend Hooks;
	friend Singleton<HookManager>;
private:
	HookManager(bool enable_now) try:
		Present(gPointers->fPresent, Hooks::Present, "Present"),
		ResizeBuffers(gPointers->fResizeBuffers, Hooks::ResizeBuffers, "ResizeBuffers"),
		SetCursorPos{L"User32", "SetCursorPos", Hooks::SetCursorPos},
		SetWindowDisplayAffinity{L"User32", "SetWindowDisplayAffinity", Hooks::SetWindowDisplayAffinity},
		GetWindowDisplayAffinity{L"User32", "GetWindowDisplayAffinity", Hooks::GetWindowDisplayAffinity}
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
private:
	MinHookCpp<decltype(Hooks::Present)> Present;
	MinHookCpp<decltype(Hooks::ResizeBuffers)> ResizeBuffers;

	MinHookCpp<decltype(Hooks::SetCursorPos)> SetCursorPos;
	MinHookCpp<decltype(Hooks::SetWindowDisplayAffinity)> SetWindowDisplayAffinity;
	MinHookCpp<decltype(Hooks::GetWindowDisplayAffinity)> GetWindowDisplayAffinity;
};

inline HookManager* gHookManager{};
