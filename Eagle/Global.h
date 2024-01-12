#pragma once

#include <Windows.h>

inline struct Global {
	HMODULE m_thisModule;
	bool m_running;

	bool m_showMenu{true};
} global{};