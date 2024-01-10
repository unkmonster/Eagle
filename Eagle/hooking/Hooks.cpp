#include "HookManager.h"
#include "Logger.h"

HMODULE WINAPI Hooks::LoadLibraryA(LPCSTR lpLibFileName) {
	auto result = gHookManager->LoadLibraryA.get_origin()(lpLibFileName);
	SPDLOG_DEBUG("Loaded {} - 0x{:X}", lpLibFileName, (uintptr_t)result);
	return result;
}

HRESULT WINAPI Hooks::Present(IDXGISwapChain* ths, UINT SyncInterval, UINT Flags) {
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
