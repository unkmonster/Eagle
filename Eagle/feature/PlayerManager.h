#pragma once

#include <vector>
#include <mutex>

#include "utils/Singleton.h"
#include "bf-1/Frosbite.h"

struct CPlayer {
	fb::ClientPlayer* m_player;
	float m_health;
	float m_maxHealth;
};

class PlayerManager {
	friend Singleton<PlayerManager>;
	friend class Esp;
	PlayerManager() = default;
public:
	void update();
	fb::ClientPlayer* GetClosetFromCrossHair(int boneId, Vec2& pos, bool onlyVisible);
private:
	const fb::ClientPlayer* m_localPlayer;
	// 对局内所有有效玩家，除本地玩家
	std::vector<CPlayer> m_players;

	std::shared_mutex mtx;
};

inline PlayerManager* gPlayerManager{};