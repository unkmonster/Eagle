#include "Sprite2d.h"

void CSprite2d::DrawOutline(const Vec2 & p1, const Vec2 & p2, uint32_t color, float thickness) {
	ImGui::GetForegroundDrawList()->AddLine(p1, p2, 0xff000000, thickness + 2.f);
	ImGui::GetForegroundDrawList()->AddLine(p1, p2, color, thickness);
}

void CSprite2d::DrawRectOutline(const Vec2 & p1, const Vec2 & p2, uint32_t color, float thickness) {
	ImGui::GetForegroundDrawList()->AddRect(p1, p2, 0xff000000, 0.f, 0, thickness + 2.f);
	ImGui::GetForegroundDrawList()->AddRect(p1, p2, color, 0.f, 0, thickness);
}

void CSprite2d::DrawRectOutline(const Vec2 & leftUp, float w, float h, uint32_t color, float thickness) {
	auto rightDown = leftUp + Vec2{w, h};
	ImGui::GetForegroundDrawList()->AddRect(leftUp, rightDown, 0xff000000, 0.f, 0, thickness + 2.f);
	ImGui::GetForegroundDrawList()->AddRect(leftUp, rightDown, color, 0.f, 0, thickness);
}

void CSprite2d::DrawCornerRectOutline(const Vec2 & p1, float w, float h, uint32_t color, float thickness) {
	const float temp = 0.25f;
	Vec2 p2{p1.x + w, p1.y + h};

	// left-up -> right-up
	DrawOutline(p1, {p1.x + w * temp, p1.y}, color);
	DrawOutline({p2.x, p1.y}, {p2.x - w * temp, p1.y}, color);

	// left-bottom -> right-bottom
	DrawOutline({p1.x, p2.y}, {p1.x + w * temp, p2.y}, color);
	DrawOutline(p2, {p2.x - w * temp, p2.y}, color);

	// left-up -> left-down
	DrawOutline(p1, {p1.x, p1.y + h * temp}, color);
	DrawOutline({p1.x, p2.y}, {p1.x, p2.y - h * temp}, color);

	// right-up -> right-down
	DrawOutline({p2.x, p1.y}, {p2.x, p1.y + h * temp}, color);
	DrawOutline(p2, {p2.x, p2.y - h * temp}, color);
}
