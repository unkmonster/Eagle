#include "Esp.h"

#include <optional>

#include <fmt/format.h>

#include "PlayerManager.h"
#include "render/Renderer.h"

void Esp::run() {
	static auto& esp_set = global.m_setting.m_esp;

	auto localPlayer = gPlayerManager->m_localPlayer;
	if (!ValidPointer(localPlayer)) return;

	// 此变量作为对是否正在游戏中的判断
	// SoldierEntity 只有玩家正在操控士兵或载具时为有效地址
	std::optional<Vec3> localOrigin;
	if (ValidPointer(localPlayer->clientSoldierEntity)) {
		if (localPlayer->InVehicle()) {
			Matrix16 matrix;
			localPlayer->clientVehicleEntity->GetTransform(&matrix);
			localOrigin = matrix.data[3];
		} else {
			localOrigin = localPlayer->clientSoldierEntity->location;
		}
	}

	uint32_t crossHairColor = ImGui::ColorConvertFloat4ToU32(esp_set.m_crossHairColor);
	Vec2 center{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};

	int enemies{};

	Vec2 closet;
	fb::ClientPlayer* closetPlayer = gPlayerManager->GetClosetFromCrossHair(fb::BONE_Head, closet, true);
	if (closetPlayer) {
		ImGui::GetBackgroundDrawList()->AddLine(center, closet, 0xff0000ff);
	}

	for (const auto& x : gPlayerManager->m_players) {
		auto player = x.m_player;
		auto soldier = player->clientSoldierEntity;
		if (player->teamId == localPlayer->teamId) continue; // 队友

		++enemies;

		//  获取当前实体位置信息
		fb::LinearTransform_AABB aabb;
		std::pair<Vec2, Vec2> minmax;
		std::vector<Vec2> points;
		Vec3 origin;
		soldier->GetTransformAABB(aabb);
		if (!fb::GetBoxPosition(aabb, points, minmax, origin)) continue;
		if (localOrigin && distance(*localOrigin, origin) > esp_set.m_effective) continue;

		auto width = minmax.second.x - minmax.first.x;
		auto height = minmax.second.y - minmax.first.y;

		// 判断当前敌人是否被瞄准
		if (esp_set.m_showCrossHair && !soldier->occluded && InBound(center, minmax.first, minmax.second))
			crossHairColor = ImGui::ColorConvertFloat4ToU32(esp_set.m_crossHairColorAtEnemy);

		//  绘制方框
		uint32_t boxColor{};
		if (player->InVehicle())
			boxColor = ImGui::ColorConvertFloat4ToU32(esp_set.m_vehicleColor);
		else if (player == closetPlayer)
			boxColor = 0xff0000ff;
		else
			boxColor = ImGui::ColorConvertFloat4ToU32(soldier->occluded? esp_set.m_boxColorOccluded : esp_set.m_boxColor);
		 
		
		if (esp_set.m_showBox) {
			if (global.m_setting.m_esp.m_boxType == BOX_FULL)
				CSprite2d::DrawRectOutline(minmax.first, minmax.second, boxColor);
			else if (global.m_setting.m_esp.m_boxType == BOX_CORNER)
				CSprite2d::DrawCornerRectOutline(minmax.first, minmax.second, boxColor);
			else if (esp_set.m_boxType == BOX_3D)
				Draw3dBox(points, boxColor);
		}

		// 绘制射线
		if (esp_set.m_showStatusLine) {
			Vec2 p1, p2;
			const auto& displaySize = ImGui::GetIO().DisplaySize;

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
		}

		// 绘制血条
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
			DrawHealthBar(x.m_health, x.m_maxHealth, p1, p2, esp_set.m_healthBarPos);
		}

		// 绘制文本信息
		std::vector<std::string> infos;
		if (esp_set.m_showName)
			infos.emplace_back(player->szName);
		if (esp_set.m_showHp)
			infos.emplace_back(fmt::format("HP: {}", static_cast<int>(x.m_health)));
		if (localOrigin && esp_set.m_showDistance)
			infos.emplace_back(fmt::format("Distance: {}M", static_cast<int>(distance(*localOrigin, origin))));

		CSprite2d::DrawTextColumnOutline({minmax.second.x + 5.f, minmax.first.y}, infos,
			gRenderer->m_textFont, global.m_setting.m_textSize, 0xFFFFFFFF);
	}

	// 绘制水印
	std::vector<std::string> texts;
	if (localOrigin) {
		texts.emplace_back(fmt::format("Enemy: {}", enemies));
		texts.emplace_back(fmt::format("Pos: {}, {}, {}", localOrigin->x, localOrigin->y, localOrigin->z));
	}
	Vec2 begin(5.f, 15.f);
	CSprite2d::DrawTextColumnOutline(begin, texts, gRenderer->m_textFont, global.m_setting.m_textSize, 0xffffffff);

	// 绘制准星
	if (localOrigin && esp_set.m_showCrossHair)
		DrawCrossHair(crossHairColor);
}
