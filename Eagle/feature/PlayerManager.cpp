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
	// 载入对局中
	if (!m_localPlayer) return;

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		CPlayer currentPlayer{};

		auto plr = currentPlayer.m_player = GetPlayerById(i);
		if (!ValidPointer(plr)) continue;
		if (plr == m_localPlayer) continue;

		auto soldier = plr->clientSoldierEntity;
		if (!ValidPointer(soldier)) continue;

		// Get health of current entity
		if (ValidPointer(soldier->healthcomponent)) {
			if (!plr->InVehicle()) {
				currentPlayer.m_health = soldier->healthcomponent->m_Health;
				currentPlayer.m_maxHealth = soldier->healthcomponent->m_MaxHealth;
			} else {
				currentPlayer.m_health = soldier->healthcomponent->m_VehicleHealth;
				if (auto data = reinterpret_cast<fb::ClientVehicleEntity*>(soldier)->GetEntityData()) {
					currentPlayer.m_maxHealth = data->m_FrontMaxHealth;
				}
			}
		}
		if (!(currentPlayer.m_health > 0.1f && currentPlayer.m_health <= currentPlayer.m_maxHealth)) continue;	// invalid entity
		m_players.emplace_back(currentPlayer);
	}
}

fb::ClientPlayer * PlayerManager::GetClosetFromCrossHair(int boneId, Vec2& pos, bool onlyVisible) {
	Vec2 center{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
	double minDistance{};
	fb::ClientPlayer* result{};

	std::shared_lock<std::shared_mutex> lock(mtx);
	for (const auto& x : m_players) {
		if (x.m_player->teamId == m_localPlayer->teamId)
			continue;
		if (x.m_player->InVehicle()) 
			continue;
		if (onlyVisible && x.m_player->clientSoldierEntity->occluded)
			continue;
		
		Vec3 bone3d;
		Vec2 bone2d;
		if (!x.m_player->clientSoldierEntity->GetBonePos(boneId, &bone3d))
			continue;
		if (!fb::w2s(&bone3d, &bone2d))
			continue;
		auto dist = distance(bone2d, center);
		if (!result || dist < minDistance) {
			minDistance = dist;
			result = x.m_player;
			pos = bone2d;
		}
	}
	return result;
}
