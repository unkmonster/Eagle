#pragma once

#include <vector>
#include <string>

#include <imgui.h>

#include "math/Vector.h"

class CSprite2d {
public:
	static void DrawOutline(const Vec2& p1, const Vec2& p2, uint32_t color, float thickness = 1.f);

	static void DrawRectOutline(const Vec2& p1, const Vec2& p2, uint32_t color, float thickness = 1.f);
	static void DrawRectOutline(const Vec2& leftUp, float w, float h, uint32_t color, float thickness = 1.f);
	static void DrawCornerRectOutline(const Vec2& p1, const Vec2& p2, uint32_t color, float thickness = 1.f);
	static void DrawCornerRectOutline(const Vec2& p1, float w, float h, uint32_t color, float thickness = 1.f);

	static void DrawTextOutline(const Vec2& pos, const std::string& text, const ImFont* font, float size, uint32_t color);

	static void DrawTextColumnOutline(Vec2 begin, const std::vector<std::string>& texts, const ImFont* font, float size, uint32_t color);
};