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

	// Functions
	// method of DXGISwapChain
	PVOID fPresent;
	PVOID fResizeBuffers;
private:
	Pointers():
		// Modules
		main("Main", GetModuleHandleA(nullptr)),
		mdxgi("dxgi.dll")
	{
		// Functions
		DBGPTR(fPresent = mdxgi.add(0x347C0).as<PVOID>());
		DBGPTR(fResizeBuffers = mdxgi.add(0x23820).as<PVOID>());
	}
};

inline Pointers* gPointers{};