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
	int count{};

	for (int i = 0; i <= 64; ++i) {
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
				health = soldier->healthcomponent->m_Health;
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

		//  -- ESP
		uint32_t boxColor = ImGui::ColorConvertFloat4ToU32(soldier->occluded? esp_set.m_boxColorOccluded : esp_set.m_boxColor);
		if (esp_set.m_showBox) {
			if (global.m_setting.m_esp.m_boxType == BOX_FULL)
				CSprite2d::DrawRectOutline(minmax.first, minmax.second, boxColor);
			else if (global.m_setting.m_esp.m_boxType == BOX_CORNER)
				CSprite2d::DrawCornerRectOutline(minmax.first, minmax.second, boxColor);
			else if (esp_set.m_boxType == BOX_3D)
				Esp::Draw3dBox(points, boxColor);
		}

		// Status Line
		if (esp_set.m_showStatusLine) {
			Vec2 p1, p2;
			auto width = minmax.second.x - minmax.first.x;
			auto height = minmax.second.y - minmax.first.y;
			auto displaySize = ImGui::GetIO().DisplaySize;

			switch (esp_set.m_statusLinePos) {
			case POS_UP:
				p1 = {displaySize.x / 2.f, 0};
				p2 = {minmax.first.x + width / 2.f, minmax.first.y};
				break;
			case POS_BOTTOM:
				p1 = {displaySize.x / 2.f, displaySize.y};
				p2 = {minmax.first.x + width / 2.f, minmax.second.y};
			}
			CSprite2d::DrawOutline(p1, p2, boxColor);
		}

		// 文本信息
		std::vector<std::string> infos;
		if (esp_set.m_showName)
			infos.emplace_back(plr->szName);
		if (esp_set.m_showHp)
			infos.emplace_back(fmt::format("HP: {}", health));
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
}

//void PlayerManager::DrawTextInfo(Vec2 begin) {
//	auto& espSet = global.m_setting.m_esp;
//
//	if (espSet.m_showName)
//}
