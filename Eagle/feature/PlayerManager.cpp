#include "PlayerManager.h"

#include "bf-1/Obfuscationmgr.h"
#include "Global.h"
#include "render/Sprite2d.h"

void PlayerManager::run() {
	m_localPlayer = GetLocalPlayer();
	if (!ValidPointer(m_localPlayer)) return;
	auto& esp_set = global.m_setting.m_esp;

	for (int i = 0; i <= 64; ++i) {
		auto plr = GetPlayerById(i);
		if (!ValidPointer(plr)) continue;
		if (plr->getId() == m_localPlayer->getId()) continue;
		if (plr->InVehicle()) continue;
		if (plr->teamId == m_localPlayer->teamId) continue;

		auto soldier = plr->clientSoldierEntity;
		if (!ValidPointer(soldier)) continue;
		if (!soldier->IsValid()) continue;

		// 计算人物坐标，尺寸
		const Vec3& origin = soldier->location;
		auto aabb = soldier->GetAABB();
		Vec3 top3d = origin + aabb.max;
		Vec3 bottom3d = origin + aabb.min;
		
		Vec2 top2d, bottom2d;
		if (!fb::w2s(&top3d, &top2d)) continue;
		if (!fb::w2s(&bottom3d, &bottom2d)) continue;

		auto height = bottom2d.y - top2d.y;
		auto width = height * 0.5f;
		Vec2 leftUp(bottom2d.x - width * 0.5f, top2d.y);

		// Features code
		if (esp_set.m_showBox) {
			if (global.m_setting.m_esp.m_boxType == BOX_FULL)
				CSprite2d::DrawRectOutline(leftUp, width, height, ImGui::ColorConvertFloat4ToU32(esp_set.m_boxColor));
			else if (global.m_setting.m_esp.m_boxType == BOX_CORNER)
				CSprite2d::DrawCornerRectOutline(leftUp, width, height, ImGui::ColorConvertFloat4ToU32(esp_set.m_boxColor));
		}

		if (esp_set.m_showName)
			ImGui::GetForegroundDrawList()->AddText(top2d, 0xffffffff, plr->name);
	}
}
