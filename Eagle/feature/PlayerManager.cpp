#include "PlayerManager.h"

#include <optional>

#include "bf-1/Obfuscationmgr.h"
#include "Global.h"
#include "render/Sprite2d.h"
#include "render/Renderer.h"
#include "Esp.h"

void PlayerManager::run() {
	m_localPlayer = GetLocalPlayer();
	if (!ValidPointer(m_localPlayer)) return;
	
	std::optional<Vec3> localOrigin;
	if (ValidPointer(m_localPlayer->clientSoldierEntity))
		localOrigin = m_localPlayer->clientSoldierEntity->location;
	
	auto& esp_set = global.m_setting.m_esp;
	Vec2 center{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
	int count{};

	uint32_t crossHairColor = ImGui::ColorConvertFloat4ToU32(global.m_setting.m_aimBot.m_crossHairColor);
	for (int i = 0; i < 64; ++i) {
		auto plr = GetPlayerById(i);
		if (!ValidPointer(plr)) continue;
		if (plr == m_localPlayer) continue;
		if (plr->teamId == m_localPlayer->teamId) continue;	// 队友

		auto soldier = plr->clientSoldierEntity;
		if (!ValidPointer(soldier)) continue;

		// Get health of current entity
		float health{}, maxHealth{};
		if (ValidPointer(soldier->healthcomponent)) {
			if (!plr->InVehicle()) {
				health = soldier->healthcomponent->m_Health;
				maxHealth = soldier->healthcomponent->m_MaxHealth;
			} else {
				health = soldier->healthcomponent->m_VehicleHealth;
				if (auto data = reinterpret_cast<fb::ClientVehicleEntity*>(soldier)->GetEntityData()) {
					maxHealth = data->m_FrontMaxHealth;
				}
			}
		}
		if (!(health > 0.1f && health <= maxHealth)) continue;	// invalid entity
		
		++count;	// players count

		//  Render Features  
		fb::LinearTransform_AABB aabb;
		std::pair<Vec2, Vec2> minmax;
		std::vector<Vec2> points;
		Vec3 origin;
		soldier->GetTransformAABB(aabb);
		if (!fb::GetBoxPosition(aabb, points, minmax, origin)) continue;
		if (localOrigin && distance(*localOrigin, origin) > esp_set.m_effective) continue;
		
		auto width = minmax.second.x - minmax.first.x;
		auto height = minmax.second.y - minmax.first.y;

		// 判断当前敌人是否在准星内
		if (global.m_setting.m_aimBot.m_showCrossHair && !soldier->occluded && InBound(center, minmax.first, minmax.second))
			crossHairColor = ImGui::ColorConvertFloat4ToU32(global.m_setting.m_aimBot.m_crossHairColorAtEnemy);

		//  -- ESP
		uint32_t boxColor = ImGui::ColorConvertFloat4ToU32(soldier->occluded? esp_set.m_boxColorOccluded : esp_set.m_boxColor);
		if (esp_set.m_showBox) {
			if (global.m_setting.m_esp.m_boxType == BOX_FULL)
				CSprite2d::DrawRectOutline(minmax.first, minmax.second, boxColor);
			else if (global.m_setting.m_esp.m_boxType == BOX_CORNER)
				CSprite2d::DrawCornerRectOutline(minmax.first, minmax.second, boxColor);
			else if (esp_set.m_boxType == BOX_3D)
				EspPage::Draw3dBox(points, boxColor);
		}

		// Status Line
		if (esp_set.m_showStatusLine) {
			Vec2 p1, p2;
			auto displaySize = ImGui::GetIO().DisplaySize;

			switch (esp_set.m_statusLinePos) {
			case POS_TOP:
				p1 = {displaySize.x / 2.f, 0};
				p2 = {minmax.first.x + width / 2.f, minmax.first.y};
				break;
			case POS_BOTTOM:
				p1 = {displaySize.x / 2.f, displaySize.y};
				p2 = {minmax.first.x + width / 2.f, minmax.second.y};
			}
			ImGui::GetBackgroundDrawList()->AddLine(p1, p2, boxColor);
			//CSprite2d::DrawOutline(p1, p2, boxColor);
		}

		// 血条
		if (esp_set.m_showHealthBar) {
			Vec2 p1, p2;
			float thickness = width / 10.f;

			if (esp_set.m_healthBarPos == POS_LEFT) {
				p2 = {minmax.first.x - 5.f, minmax.second.y};
				p1 = {p2.x - thickness, minmax.first.y};
			} else if (esp_set.m_healthBarPos == POS_RIGHT) {
				p1 = {minmax.second.x + 5.f, minmax.first.y};
				p2 = {p1.x + thickness, minmax.second.y};
			} else if (esp_set.m_healthBarPos == POS_TOP) {
				p2 = {minmax.second.x, minmax.first.y - 5.f};
				p1 = {minmax.first.x, p2.y - thickness};
			} else {
				p1 = {minmax.first.x, minmax.second.y + 5.f};
				p2 = {minmax.second.x, p1.y + thickness};
			}
			EspPage::DrawHealthBar(health, maxHealth,p1, p2, esp_set.m_healthBarPos);
		}

		// 文本信息
		std::vector<std::string> infos;
		if (esp_set.m_showName)
			infos.emplace_back(plr->szName);
		if (esp_set.m_showHp)
			infos.emplace_back(fmt::format("HP: {}", static_cast<int>(health)));
		if (esp_set.m_showDistance && localOrigin.has_value())
			infos.emplace_back(fmt::format("Distance: {}M", static_cast<int>(distance(localOrigin.value(), origin))));

		auto textColor = ImGui::ColorConvertFloat4ToU32(global.m_setting.m_esp.m_textColor);
		CSprite2d::DrawTextColumnOutline({minmax.second.x + 5.f, minmax.first.y}, infos, 
			gRenderer->m_textFont, global.m_setting.m_textSize, textColor);
	}

	// 水印
	std::vector<std::string> texts{
		fmt::format("Enemy: {}", count),
	};
	if (localOrigin)
		texts.emplace_back(fmt::format("Pos: {}, {}, {}", localOrigin->x, localOrigin->y, localOrigin->z));

	Vec2 begin(5.f, 15.f);
	CSprite2d::DrawTextColumnOutline(begin, texts, gRenderer->m_textFont, global.m_setting.m_textSize, 0xffffffff);

	// 准星
	if (localOrigin && global.m_setting.m_aimBot.m_showCrossHair)
		EspPage::DrawCrossHair(crossHairColor);
}

//void PlayerManager::DrawTextInfo(Vec2 begin) {
//	auto& espSet = global.m_setting.m_esp;
//
//	if (espSet.m_showName)
//}
