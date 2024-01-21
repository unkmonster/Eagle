#pragma once

#include <vector>
#include <mutex>

#include "utils/Singleton.h"
#include "bf-1/Frosbite.h"

class PlayerManager {
	friend Singleton<PlayerManager>;
	friend class Esp;
	PlayerManager() = default;
public:
	void update();
	fb::ClientPlayer* GetClosetFromCrossHair(int boneId, Vec2& pos, bool onlyVisible);
	std::vector<char*> observers() const;
private:
	const fb::ClientPlayer* m_localPlayer;
	std::vector<fb::ClientPlayer*> m_players;

	std::shared_mutex mtx;
};

inline PlayerManager* gPlayerManager{};