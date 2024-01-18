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

	static POINT cursorPos;

	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_INSERT) {
			if (global.m_showMenu == true)
				GetCursorPos(&cursorPos);
			else
				SetCursorPos(cursorPos.x, cursorPos.y);
			ImGui::GetIO().MouseDrawCursor = global.m_showMenu ^= 1;
		}
		else if (wParam == VK_END)
			std::thread(FreeLibrary, global.m_thisModule).detach();
		break;

	// ÆÁ±ÎÓÎÏ·Êó±êÊäÈë
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		if (global.m_showMenu)
			return 0;
	}
	return CallWindowProcA(gPointers->oWndproc, hwnd, msg, wParam, lParam);
}

BOOL __stdcall Hooks::SetCursorPos(int X, int Y) {
	if (global.m_showMenu)
		return TRUE;
	return gHookManager->SetCursorPos.call_origin(X, Y);
}