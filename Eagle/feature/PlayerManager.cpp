#include "PlayerManager.h"

#include "bf-1/Obfuscationmgr.h"
#include "Global.h"
#include "render/Sprite2d.h"
#include "render/Renderer.h"
#include "Esp.h"

void PlayerManager::run() {
	m_localPlayer = GetLocalPlayer();
	if (!ValidPointer(m_localPlayer)) return;
	
	auto& esp_set = global.m_setting.m_esp;
	int count{};

	for (int i = 0; i <= 64; ++i) {
		auto plr = GetPlayerById(i);
		if (!ValidPointer(plr)) continue;
		if (plr == m_localPlayer) continue;
		//if (!plr->InVehicle()) continue;
		if (plr->teamId == m_localPlayer->teamId) continue;	// 队友

		auto soldier = plr->clientSoldierEntity;
		if (!ValidPointer(soldier)) continue;
		//if (!soldier->IsValid()) continue;
		++count;

		fb::LinearTransform_AABB aabb;
		soldier->GetTransformAABB(aabb);
		std::pair<Vec2, Vec2> minmax;
		std::vector<Vec2> points;
		if (!fb::GetBoxPosition(aabb, points, minmax)) continue;

		
		// Features code
		if (esp_set.m_showBox) {
			uint32_t color = ImGui::ColorConvertFloat4ToU32(soldier->occluded? esp_set.m_boxColorOccluded : esp_set.m_boxColor);

			if (global.m_setting.m_esp.m_boxType == BOX_FULL)
				CSprite2d::DrawRectOutline(minmax.first, minmax.second, color);
			else if (global.m_setting.m_esp.m_boxType == BOX_CORNER)
				CSprite2d::DrawCornerRectOutline(minmax.first, minmax.second, color);
			else if (esp_set.m_boxType == BOX_3D)
				Esp::Draw3dBox(points, color);
		}

		std::vector<std::string> infos;
		//// 文本信息显示
		//if (esp_set.m_showName)
		//	infos.emplace_back(fmt::format("name: {}", plr->szName));
		//if (esp_set.m_showHp)
		//	infos.emplace_back(fmt::format("HP: {}", soldier->healthcomponent->m_Health));
		//if (esp_set.m_showDistance && ValidPointer(m_localPlayer->clientSoldierEntity)) {
		//	Vec2 selfPos = m_localPlayer->InVehicle() ? 
		//		selfPos = m_localPlayer->clientVehicleEntity->GetVehiclePosition() :
		//		selfPos = m_localPlayer->clientSoldierEntity->location;
		//	int dist = static_cast<int>(distance(selfPos, soldier->location));
		//	infos.emplace_back(fmt::format("Distance: {}M", dist));
		//}
		//	
		//Vec2 begin{leftUp.x + width + gRenderer->m_fontSize, leftUp.y};
		//for (const auto& x : infos) {
		//	CSprite2d::DrawTextOutline(begin,
		//		x,
		//		gRenderer->m_textFont, global.m_setting.m_textSize,
		//		ImGui::ColorConvertFloat4ToU32(esp_set.m_textColor));
		//	begin.y += global.m_setting.m_textSize;
		//}
	}

	// 水印
	Vec2 begin(5.f, 15.f);
	CSprite2d::DrawTextOutline(begin,
		fmt::format("Enemy: {}", count), 
		gRenderer->m_textFont, global.m_setting.m_textSize, 0xffffffff);
	begin.y += global.m_setting.m_textSize;

	if (ValidPointer(m_localPlayer->clientSoldierEntity)) {
		Vec3 pos;
		if (m_localPlayer->InVehicle()) pos = m_localPlayer->clientVehicleEntity->GetVehiclePosition();
		else pos = m_localPlayer->clientSoldierEntity->location;
		CSprite2d::DrawTextOutline(begin,
			fmt::format("Pos: {}, {}, {}", pos.x, pos.y, pos.z),
			gRenderer->m_textFont, global.m_setting.m_textSize, 0xffffffff);
	}
}

//void PlayerManager::DrawTextInfo(Vec2 begin) {
//	auto& espSet = global.m_setting.m_esp;
//
//	if (espSet.m_showName)
//}
