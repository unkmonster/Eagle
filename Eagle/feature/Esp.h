#pragma once

#include <vector>

#include "render/Sprite2d.h"
#include "Global.h"

class Esp {
public:
	static void Draw3dBox(const std::vector<Vec2>& points, uint32_t color) {
		// 暂时固定点位吧
		auto dl = ImGui::GetBackgroundDrawList();

		dl->AddLine(points[1], points[3], color);
		dl->AddLine(points[3], points[5], color);
		dl->AddLine(points[5], points[7], color);
		dl->AddLine(points[7], points[1], color);

		dl->AddLine(points[0], points[2], color);
		dl->AddLine(points[2], points[6], color);
		dl->AddLine(points[6], points[4], color);
		dl->AddLine(points[4], points[0], color);

		dl->AddLine(points[1], points[6], color);
		dl->AddLine(points[7], points[2], color);
		dl->AddLine(points[3], points[4], color);
		dl->AddLine(points[5], points[0], color);
	}

	static void DrawHealthBar(float health, float maxHealth, const Vec2& p1, const Vec2& p2, PosType pos) {
		auto dl = ImGui::GetBackgroundDrawList();
		
		// 计算血条颜色
		uint32_t color = 0xFF00FF00;
		if (health <= maxHealth * 0.25f)
			color = 0xff0000ff;
		else if (health <= maxHealth * 0.5f)
			color = 0xff00a5ff;
		else if (health <= maxHealth * 0.75f)
			color = 0xff00ffff;
		
		float length{}, healthLength{};

		if (pos == POS_LEFT || pos == POS_RIGHT) {
			/*---------bottom -> top-------------*/
			length = fabs(p2.y - p1.y) - 2.f;
			healthLength = length * health / maxHealth;
			dl->AddRectFilled(ImVec2{p1.x, p2.y - healthLength}, p2, color);
			if (fabs(p2.x - p1.x) >= 4.f)
				dl->AddRect(p1, p2, 0xff000000);
		} else {
			length = fabs(p1.x - p2.x);
			healthLength = length * health / maxHealth;
			dl->AddRectFilled(p1, {p1.x + healthLength, p2.y}, color);
			if (fabs(p2.y - p1.y) >= 4.f)
				dl->AddRect(p1, p2, 0xff000000);
		}
	}
};