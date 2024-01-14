#pragma once

#include <Windows.h>

#include "Setting.h"

inline struct Global {
	HMODULE m_thisModule;

	Setting m_setting;

	bool m_showMenu{true};
} global{};