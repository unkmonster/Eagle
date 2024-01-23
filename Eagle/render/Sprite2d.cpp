#include "Sprite2d.h"

void CSprite2d::DrawOutline(const Vec2 & p1, const Vec2 & p2, uint32_t color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddLine(p1, p2, 0xff000000 & color, thickness + 2.f);
	ImGui::GetBackgroundDrawList()->AddLine(p1, p2, color, thickness);
}

void CSprite2d::DrawRectOutline(const Vec2 & p1, const Vec2 & p2, uint32_t color, float thickness) {
	ImGui::GetBackgroundDrawList()->AddRect(p1, p2, 0xff000000 & color, 0.f, 0, thickness + 2.f);
	ImGui::GetBackgroundDrawList()->AddRect(p1, p2, color, 0.f, 0, thickness);
}

void CSprite2d::DrawRectOutline(const Vec2 & leftUp, float w, float h, uint32_t color, float thickness) {
	auto rightDown = leftUp + Vec2{w, h};
	ImGui::GetBackgroundDrawList()->AddRect(leftUp, rightDown, 0xff000000 & color, 0.f, 0, thickness + 2.f);
	ImGui::GetBackgroundDrawList()->AddRect(leftUp, rightDown, color, 0.f, 0, thickness);
}

void CSprite2d::DrawCornerRectOutline(const Vec2 & p1, const Vec2 & p2, uint32_t color, float thickness) {
	DrawCornerRectOutline(p1, p2.x - p1.x, p2.y - p1.y, color, thickness);
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

void CSprite2d::DrawTextOutline(const Vec2 & pos, const std::string& text, const ImFont * font, float size, uint32_t color) {
	auto dl = ImGui::GetBackgroundDrawList();

	dl->AddText(font, size, {pos.x - 1, pos.y - 1}, 0xFF000000 & color, text.c_str());
	dl->AddText(font, size, {pos.x + 1, pos.y - 1}, 0xFF000000 & color, text.c_str());
	dl->AddText(font, size, {pos.x - 1, pos.y + 1}, 0xFF000000 & color, text.c_str());
	dl->AddText(font, size, {pos.x + 1, pos.y + 1}, 0xFF000000 & color, text.c_str());

	/*dl->AddText(font, size, {pos.x - 1, pos.y}, 0xFF000000, text.c_str());
	dl->AddText(font, size, {pos.x + 1, pos.y}, 0xFF000000, text.c_str());
	dl->AddText(font, size, {pos.x, pos.y + 1}, 0xFF000000, text.c_str());
	dl->AddText(font, size, {pos.x, pos.y - 1}, 0xFF000000, text.c_str());*/
	dl->AddText(font, size, pos, color, text.c_str());
}

void CSprite2d::DrawTextColumnOutline(Vec2 begin, const std::vector<std::string>& texts, const ImFont * font, float size, uint32_t color) {
	for (const auto& x : texts) {
		DrawTextOutline(begin, x, font, size, color);
		begin.y += size;
	}
}
