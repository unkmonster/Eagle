#pragma once

#include <vector>

#include "utils/Singleton.h"
#include "bf-1/Frosbite.h"

class PlayerManager {
	friend Singleton<PlayerManager>;
	PlayerManager() = default;
public:
	void run();
public:
	const fb::ClientPlayer* m_localPlayer;
	const fb::ClientPlayer* m_closestFromCrosshair;
};

inline PlayerManager* gPlayerManager{};