#include "Esp.h"

#include <optional>
#include <utility>

#include <fmt/format.h>

#include "PlayerManager.h"
#include "render/Renderer.h"

void Esp::run() {
	static auto& esp_set = global.m_setting.m_esp;

	auto localPlayer = gPlayerManager->m_localPlayer;
	if (!ValidPointer(localPlayer)) return;

	// 此变量作为对是否正在游戏中的判断
	// SoldierEntity 只有玩家正在操控士兵或载具时为有效地址
	std::optional<Vec3> localOrigin = localPlayer->GetPosition();
	
	bool targeted{};
	uint32_t vColor = ImGui::ColorConvertFloat4ToU32(esp_set.m_vehicleColor);
	uint32_t color = ImGui::ColorConvertFloat4ToU32(esp_set.m_boxColor);
	uint32_t oColor = ImGui::ColorConvertFloat4ToU32(esp_set.m_boxColorOccluded);

	for (const auto& x : gPlayerManager->m_players) {
		draw(x, false, esp_set.m_effective, localOrigin, targeted, vColor, color, oColor, 
			esp_set.m_showBox, esp_set.m_showStatusLine, esp_set.m_showHealthBar, true);
	}

	// 绘制水印
	std::vector<std::string> texts;
	if (localOrigin) {
		texts.emplace_back(fmt::format("Pos: {}, {}, {}", localOrigin->x, localOrigin->y, localOrigin->z));
	}
	Vec2 begin(5.f, 15.f);
	CSprite2d::DrawTextColumnOutline(begin, texts, gRenderer->m_textFont, global.m_setting.m_textSize, 0xffffffff);

	// 绘制准星
	if (localOrigin && esp_set.m_showCrossHair)
		DrawCrossHair(ImGui::ColorConvertFloat4ToU32(!targeted? esp_set.m_crossHairColor: esp_set.m_crossHairColorAtEnemy));

	// 绘制自瞄有效范围
	if (localOrigin && global.m_setting.m_aimBot.m_showArea) {
		Vec2 center{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
		auto x = global.m_setting.m_aimBot.m_areaSize.x / 2.f;
		auto y = global.m_setting.m_aimBot.m_areaSize.y / 2.f;
		Vec2 offset{x, y};
		auto color = ImGui::ColorConvertFloat4ToU32(global.m_setting.m_aimBot.m_areaColor);
		CSprite2d::DrawCornerRectOutline(center - offset, center + offset, color);
	}
}

void Esp::draw(
	const fb::ClientPlayer * plr, 
	bool includeTeammate, 
	int effective, 
	const std::optional<Vec3>& localOrigin, 
	bool& targeted, 
	uint32_t vehicleColor, 
	uint32_t visibleColor, 
	uint32_t invisibleColor, 
	bool showBox, 
	bool showLine, 
	bool showHealth, 
	bool showText) 
{
	static auto& set = global.m_setting.m_esp;

	if (!gPlayerManager->m_localPlayer) return;
	if (!plr) return;
	if (!plr->clientSoldierEntity) return;
	if (!includeTeammate && plr->teamId == gPlayerManager->m_localPlayer->teamId) return;

	Vec2 center{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
	
	auto soldier = plr->clientSoldierEntity;

	// 获取血量信息
	std::optional<std::pair<float, float>> health;
	{
		std::pair<float, float> temp;
		if (plr->InVehicle()) {
			if (plr->clientVehicleEntity->GetHealth(temp))
				health = std::move(temp);
		} else {
			if (soldier->GetHealth(temp))
				health = std::move(temp);
		}
	}
	if (!health || health->first < 0.1f || health->first > health->second)
		return;

	//  获取实体 3D, 2D 位置信息
	fb::LinearTransform_AABB aabb;
	std::pair<Vec2, Vec2> minmax;
	std::vector<Vec2> points;
	Vec3 origin;
	double dist{};
	soldier->GetTransformAABB(aabb);
	if (!fb::GetBoxPosition(aabb, points, minmax, origin)) return;
	if (localOrigin && (dist = distance(*localOrigin, origin)) > effective)
		return;

	auto width = minmax.second.x - minmax.first.x;
	auto height = minmax.second.y - minmax.first.y;


	// 判断当前敌人是否被瞄准
	if (!soldier->occluded && InBound(center, minmax.first, minmax.second))
		targeted = true;

	// 判断颜色
	uint32_t boxColor{};
	if (plr->InVehicle())
		boxColor = vehicleColor;
	else
		boxColor = soldier->occluded? invisibleColor : visibleColor;


	//  绘制方框
	if (showBox) {
		if (set.m_boxType == BOX_FULL)
			CSprite2d::DrawRectOutline(minmax.first, minmax.second, boxColor);
		else if (set.m_boxType == BOX_CORNER)
			CSprite2d::DrawCornerRectOutline(minmax.first, minmax.second, boxColor);
		else if (set.m_boxType == BOX_3D)
			Draw3dBox(points, boxColor);
	}

	// 绘制射线
	if (showLine) {
		Vec2 p1, p2;
		const auto& displaySize = ImGui::GetIO().DisplaySize;

		switch (set.m_statusLinePos) {
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
	if (showHealth && health) {
		Vec2 p1, p2;
		float thickness = width / 20.f;

		if (set.m_healthBarPos == POS_LEFT) {
			p2 = {minmax.first.x - 5.f, minmax.second.y};
			p1 = {p2.x - thickness, minmax.first.y};
		} else if (set.m_healthBarPos == POS_RIGHT) {
			p1 = {minmax.second.x + 5.f, minmax.first.y};
			p2 = {p1.x + thickness, minmax.second.y};
		} else if (set.m_healthBarPos == POS_TOP) {
			p2 = {minmax.second.x, minmax.first.y - 5.f};
			p1 = {minmax.first.x, p2.y - thickness};
		} else {
			p1 = {minmax.first.x, minmax.second.y + 5.f};
			p2 = {minmax.second.x, p1.y + thickness};
		}
		DrawHealthBar(health->first, health->second, p1, p2, set.m_healthBarPos);
	}

	// 绘制文本信息
	if (showText) {
		std::vector<std::string> infos;
		if (set.m_showName)
			infos.emplace_back(plr->szName);
		if (set.m_showHp && health)
			infos.emplace_back(fmt::format("HP: {}", static_cast<int>(health->second)));
		if (localOrigin && set.m_showDistance)
			infos.emplace_back(fmt::format("{}M", static_cast<uint64_t>(dist)));
		if (plr->InVehicle()) {
			auto ed = plr->clientVehicleEntity->GetEntityData();
			if (ValidPointer(ed) && ValidPointer(ed->m_namesid))
				infos.emplace_back(ed->m_namesid);
		}

		CSprite2d::DrawTextColumnOutline({minmax.second.x + 5.f, minmax.first.y}, infos,
			gRenderer->m_textFont, global.m_setting.m_textSize, 0xFFFFFFFF);
	}
	return;
}
