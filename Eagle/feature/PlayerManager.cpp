#include "PlayerManager.h"

#include <optional>

#include "bf-1/Obfuscationmgr.h"
#include "Global.h"
#include "render/Sprite2d.h"
#include "render/Renderer.h"
#include "Esp.h"

void PlayerManager::update() {
	std::lock_guard<std::shared_mutex> lock(mtx);
	m_players.clear();

	m_localPlayer = GetLocalPlayer();
	//// 载入对局中
	//if (!m_localPlayer) return;

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		auto plr = GetPlayerById(i);
		if (!ValidPointer(plr)) continue;
		if (plr == m_localPlayer) continue;
		m_players.emplace_back(plr);
	}
}

fb::ClientPlayer * PlayerManager::GetClosetFromCrossHair(int boneId, Vec2& pos, bool onlyVisible) {
	Vec2 center{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
	double minDistance{};
	fb::ClientPlayer* result{};

	std::shared_lock<std::shared_mutex> lock(mtx);
	for (const auto& x : m_players) {
		if (!ValidPointer(x->clientSoldierEntity)) continue;
		if (x->teamId == m_localPlayer->teamId) continue;
		if (x->InVehicle() && !x->clientVehicleEntity->IsHorse()) continue;
		if (onlyVisible && x->clientSoldierEntity->occluded) continue;
		std::pair<float, float> hp;
		if (!x->clientSoldierEntity->GetHealth(hp)) continue;
		if (hp.first < 0.1f || hp.first > hp.second) continue;
		
		Vec3 bone3d;
		Vec2 bone2d;
		if (!x->clientSoldierEntity->GetBonePos(boneId, &bone3d))
			continue;
		if (!fb::w2s(&bone3d, &bone2d))
			continue;
		auto dist = distance(bone2d, center);
		if (!result || dist < minDistance) {
			minDistance = dist;
			result = x;
			pos = bone2d;
		}
	}
	return result;
}

std::vector<char*> PlayerManager::observers() const {
	std::vector<char*> result;
	for (const auto& x : m_players) {
		if (x->isSpectator)
			result.push_back(x->name);
	}
	if (ValidPointer(m_localPlayer) && m_localPlayer->isSpectator)
		result.push_back(m_localPlayer->name);
	return result;
}

std::vector<std::string> PlayerManager::debug_info() const {
	std::vector<std::string> result;
	result.emplace_back(fmt::format("localPlayer: 0x{:X}", uintptr_t(m_localPlayer)));
	return result;
}
