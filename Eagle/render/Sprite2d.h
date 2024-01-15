#pragma once

#include "math/Vector.h"

class CSprite2d {
public:
	static void DrawOutline(const Vec2& p1, const Vec2& p2, uint32_t color, float thickness = 1.f);
	static void DrawRectOutline(const Vec2& p1, const Vec2& p2, uint32_t color, float thickness = 1.f);
	static void DrawRectOutline(const Vec2& leftUp, float w, float h, uint32_t color, float thickness = 1.f);
	static void DrawCornerRectOutline(const Vec2& p1, float w, float h, uint32_t color, float thickness = 1.f);
};