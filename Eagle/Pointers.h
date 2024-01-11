#pragma once

#include <Windows.h>
#include <cassert>

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

	// Functions
	// method of DXGISwapChain
	PVOID fPresent;
	PVOID fResizeBuffers;

	WNDPROC oWndproc;
	HWND hwnd;
private:
	Pointers():
		// Modules
		main("Main", GetModuleHandleA(nullptr)),
		mdxgi("dxgi.dll"),
		mGameOverLay("gameoverlayrenderer64.dll")
	{
		assert(hwnd = FindWindow(TEXT("Battlefield™ 1"), nullptr));
		assert(oWndproc = reinterpret_cast<WNDPROC>(GetWindowLongPtrA(hwnd, GWLP_WNDPROC)));
		
		// Functions
		fPresent = Pattern("48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 41 8B E8", 
			"IDXGISwapChain::Present").find(mGameOverLay).as<PVOID>();
		assert(fResizeBuffers = mdxgi.add(0x23820).as<PVOID>());
	}
};

inline Pointers* gPointers{};