#include <Windows.h>
#include <mutex>
#include <stdexcept>
#include <thread>

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

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


// IDXGISwapChain
HRESULT WINAPI Hooks::Present(IDXGISwapChain* ths, UINT SyncInterval, UINT Flags) {
	gRenderer->on_present();	// must be not nullptr
	return gHookManager->Present.call_origin(ths, SyncInterval, Flags);
}

HRESULT __stdcall Hooks::ResizeBuffers(
	IDXGISwapChain* ths,
	UINT BufferCount, 
	UINT Width, UINT Height, 
	DXGI_FORMAT NewFormat, 
	UINT SwapChainFlags
) {
	gRenderer->ReleaseRenderView();
	//ImGui_ImplDX11_CreateDeviceObjects();

	auto rst = gHookManager->ResizeBuffers.call_origin(
		ths, BufferCount, 
		Width, Height, 
		NewFormat, SwapChainFlags
	);
	
	if (SUCCEEDED(rst)) {
		gRenderer->CreateRenderView(gPointers->pSwapChain);
		//ImGui_ImplDX11_InvalidateDeviceObjects();
	}
	return rst;
}


LRESULT CALLBACK Hooks::WNDPROC(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_INSERT)
			global.m_showMenu ^= 1;
		else if (wParam == VK_END)
			std::thread(FreeLibrary, global.m_thisModule).detach();
			//global.m_running = false;
		break;

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		if (global.m_showMenu)
			return 0;
	}
	return CallWindowProcA(gPointers->oWndproc, hwnd, msg, wParam, lParam);
}
