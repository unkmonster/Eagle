#pragma once

#include <Windows.h>
#include <cassert>

#include "utils/Utility.h"
#include "Logger.h"
#include "memory/Module.h"
#include "memory/Pattern.h"
#include "Global.h"

#define DLLMODULE(name) Module m##name = Module(#name)

struct Pointers {
	friend class Singleton<Pointers>;
public:
	Module mian;

	Pointer test;

	~Pointers() {
		SPDLOG_DEBUG(__func__);
	}
private:
	Pointers():
		mian("Main", GetModuleHandleA(nullptr)),
		test(Pattern("48 83 EC 28 E8 ? ? ? ? 48 83 C4 28 E9 ? ? ? ?", "test").find(mian).add(14).rip())
	{
		SPDLOG_DEBUG("0x{:X}", test.as<uintptr_t>());
	}
};

inline Pointers* gPointers{};