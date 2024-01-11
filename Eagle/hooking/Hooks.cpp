#include <Windows.h>
#include <mutex>
#include <stdexcept>

#include <imgui_impl_win32.h>

#include "HookManager.h"
#include "Logger.h"
#include "Pointers.h"
#include "render/Renderer.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HMODULE WINAPI Hooks::LoadLibraryA(LPCSTR lpLibFileName) {
	auto result = gHookManager->LoadLibraryA.get_origin()(lpLibFileName);
	SPDLOG_DEBUG("Loaded {} - 0x{:X}", lpLibFileName, (uintptr_t)result);
	return result;
}

HRESULT WINAPI Hooks::Present(IDXGISwapChain* ths, UINT SyncInterval, UINT Flags) {
	static std::once_flag flag;
	std::call_once(flag, [ths]() {Renderer::chain_pms.set_value(ths);});

	if (Singleton<Renderer>::initialize())
		Singleton<Renderer>::initialize()->on_present();

	return gHookManager->Present.get_origin()(ths, SyncInterval, Flags);
}

HRESULT __stdcall Hooks::ResizeBuffers(
	IDXGISwapChain* ths,
	UINT BufferCount, 
	UINT Width, UINT Height, 
	DXGI_FORMAT NewFormat, 
	UINT SwapChainFlags
) {
	SPDLOG_DEBUG("");
	return gHookManager->ResizeBuffers.call_origin(ths, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

LRESULT CALLBACK Hooks::WNDPROC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;
	return CallWindowProcA(gPointers->oWndproc, hwnd, msg, wParam, lParam);
}
