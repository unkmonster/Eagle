#pragma once

#include <vector>

#include "render/Sprite2d.h"
#include "Global.h"

class Esp {
public:
	static void Draw3dBox(const std::vector<Vec2>& points, uint32_t color) {
		// 暂时固定点位吧
		CSprite2d::DrawOutline(points[1], points[3], color);
		CSprite2d::DrawOutline(points[3], points[5], color);
		CSprite2d::DrawOutline(points[5], points[7], color);
		CSprite2d::DrawOutline(points[7], points[1], color);

		CSprite2d::DrawOutline(points[0], points[2], color);
		CSprite2d::DrawOutline(points[2], points[6], color);
		CSprite2d::DrawOutline(points[6], points[4], color);
		CSprite2d::DrawOutline(points[4], points[0], color);

		CSprite2d::DrawOutline(points[1], points[6], color);
		CSprite2d::DrawOutline(points[7], points[2], color);
		CSprite2d::DrawOutline(points[3], points[4], color);
		CSprite2d::DrawOutline(points[5], points[0], color);
	}
};