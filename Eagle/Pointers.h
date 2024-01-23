#pragma once

#include <Windows.h>
#include <cassert>
#include <typeinfo>
#include <d3d11.h>

#include "utils/Utility.h"
#include "Logger.h"
#include "memory/Module.h"
#include "memory/Pattern.h"
#include "Global.h"

#define DBGPTR(name) spdlog::debug("[0x{1:X}] {0} ", #name, (uintptr_t)(name))

struct Pointers {
	friend class Singleton<Pointers>;
public:
	Module main;
	Module mdxgi;
	Module mGameOverLay;

	WNDPROC oWndproc;
	HWND hwnd;

	// method of DXGISwapChain
	IDXGISwapChain* pSwapChain;
	// Functions
	PVOID fPresent;
	PVOID fResizeBuffers;
private:
	Pointers():
		// Modules
		main(GetModuleHandleA(nullptr)),
		mdxgi("dxgi.dll"),
		mGameOverLay("gameoverlayrenderer64.dll") 
	{
		assert(hwnd = FindWindow(TEXT("Battlefield™ 1"), nullptr));
		assert(oWndproc = reinterpret_cast<WNDPROC>(GetWindowLongPtrA(hwnd, GWLP_WNDPROC)));
		
		pSwapChain = Pattern("48 8B 0D ? ? ? ? 48 8B D8 4C 89 71 18", "IDXGISwapChain*").
			find(mGameOverLay).add(3).rip().as<Pointer&>().add(0x18).as<IDXGISwapChain*&>();
		
		// Functions
		assert(fPresent = mdxgi.add(0x347C0).as<PVOID>());
		assert(fResizeBuffers = mdxgi.add(0x23820).as<PVOID>());
	}
};

inline Pointers* gPointers{};