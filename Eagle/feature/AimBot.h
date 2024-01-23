#pragma once

#include <future>
#include <atomic>
#include <string>

#include "PlayerManager.h"
#include "Pointers.h"
#include "Esp.h"

class AimBot {
public:
	static void run();
private:
	static std::atomic_bool m_ready;
};