#pragma once

#include <Windows.h>
#include <cassert>

#include "Utility.h"
#include "Logger.h"
#include "memory/Module.h"

#define DLLMODULE(name) Module m##name = Module(#name)

struct Pointers {
	friend class Singleton<Pointers>;

	// module
	Module mKERNEL32;

	~Pointers() {
		fmt::output_debug(__func__);
	}
private:
	Pointers():
		mKERNEL32("KERNEL32") {
		SPDLOG_DEBUG("{} {} -- 0x{:X}", mKERNEL32.name(), mKERNEL32.size(), mKERNEL32.get<uintptr_t>());
	}
};

inline Pointers* gPointers{};