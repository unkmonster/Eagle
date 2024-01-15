#include "PlayerManager.h"

#include "bf-1/Obfuscationmgr.h"
#include "Global.h"
#include "render/Sprite2d.h"
#include "render/Renderer.h"

void PlayerManager::run() {
	m_localPlayer = GetLocalPlayer();
	if (!ValidPointer(m_localPlayer)) return;
	
	auto& esp_set = global.m_setting.m_esp;
	int count{};

	for (int i = 0; i <= 64; ++i) {
		auto plr = GetPlayerById(i);
		if (!ValidPointer(plr)) continue;
		if (plr->getId() == m_localPlayer->getId()) continue;
		if (plr->InVehicle()) continue;	// TODO
		if (plr->teamId == m_localPlayer->teamId) continue;

		auto soldier = plr->clientSoldierEntity;
		if (!ValidPointer(soldier)) continue;
		if (!soldier->IsValid()) continue;

		std::vector<std::string> infos;
		++count;

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
			uint32_t color = ImGui::ColorConvertFloat4ToU32(soldier->occluded? esp_set.m_boxColorOccluded : esp_set.m_boxColor);

			if (global.m_setting.m_esp.m_boxType == BOX_FULL)
				CSprite2d::DrawRectOutline(leftUp, width, height, color);
			else if (global.m_setting.m_esp.m_boxType == BOX_CORNER)
				CSprite2d::DrawCornerRectOutline(leftUp, width, height, color);
		}


		// 文本信息显示
		if (esp_set.m_showName)
			infos.emplace_back(fmt::format("name: {}", plr->szName));
		if (esp_set.m_showHp)
			infos.emplace_back(fmt::format("HP: {}", soldier->healthcomponent->m_Health));
		if (esp_set.m_showDistance && ValidPointer(m_localPlayer->clientSoldierEntity)) {
			Vec2 selfPos = m_localPlayer->InVehicle() ? 
				selfPos = m_localPlayer->clientVehicleEntity->GetVehiclePosition() :
				selfPos = m_localPlayer->clientSoldierEntity->location;
			int dist = static_cast<int>(distance(selfPos, soldier->location));
			infos.emplace_back(fmt::format("Distance: {}M", dist));
		}
			
		Vec2 begin{leftUp.x + width + gRenderer->m_fontSize, leftUp.y};
		for (const auto& x : infos) {
			CSprite2d::DrawTextOutline(begin,
				x,
				gRenderer->m_textFont, global.m_setting.m_textSize,
				ImGui::ColorConvertFloat4ToU32(esp_set.m_textColor));
			begin.y += global.m_setting.m_textSize;
		}
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
