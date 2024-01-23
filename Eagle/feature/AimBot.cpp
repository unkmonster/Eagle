#include "AimBot.h"

std::atomic_bool AimBot::m_ready = true;

namespace {
fb::BoneIds bones[] = {
	fb::BONE_Head,
	fb::BONE_Neck,
	fb::BONE_SPINE2,
	fb::BONE_SPINE1,
	fb::BONE_Spine
};
}

void AimBot::run() {
	using namespace std::chrono_literals;
	constexpr int maxDist = 30;
	static fb::ClientPlayer* prev{};

	Vec2 pos;
	auto plr = gPlayerManager->GetClosetFromCrossHair(bones[global.m_setting.m_aimBot.m_bone], pos, global.m_setting.m_aimBot.m_onlyVisible);
	if (!ValidPointer(plr)) return;

	Vec2 cursor{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
	auto min = cursor - Vec2{global.m_setting.m_aimBot.m_areaSize.x / 2.f, global.m_setting.m_aimBot.m_areaSize.y / 2.f};
	auto max = min + global.m_setting.m_aimBot.m_areaSize;
	if (!InBound(pos, min, max)) return;

	auto selfPos = gPlayerManager->m_localPlayer->GetPosition();
	if (gPlayerManager->m_localPlayer) {
		auto targetPos = plr->GetPosition();
		if (selfPos && targetPos && distance(*selfPos, *targetPos) > global.m_setting.m_aimBot.m_maxDistance)
			return;
	}

	// Cold Down
	if (prev && prev != plr) {
		m_ready = false;
		SPDLOG_DEBUG("AIMBOT COOLDOWN");
		std::thread([]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(global.m_setting.m_aimBot.m_coldDown));
			m_ready = true;
		}).detach();
		prev = nullptr;
	}
	if (!m_ready) return;

	bool tmp{};
	Esp::draw(plr, false, global.m_setting.m_aimBot.m_maxDistance, selfPos, tmp,
		0xff0000ff, 0xff0000ff, 0xff0000ff, true, false, global.m_setting.m_esp.m_showHealthBar, true);
	ImGui::GetBackgroundDrawList()->AddLine(cursor, pos, 0xff0000ff);
	ImGui::GetBackgroundDrawList()->AddCircle(pos, 1.f, 0xffffffff);

	if (!GetAsyncKeyState(VK_RBUTTON)) return;

#ifdef _DEBUG
	static std::string last{};
	if (last != plr->szName) {
		SPDLOG_DEBUG("{}", plr->name);
		last = plr->name;
	}
#endif // _DEBUG

	// º∆À„“∆∂Øæ‡¿Î
	std::pair<short, short> diff{
		static_cast<short>(pos.x - cursor.x),
		static_cast<short>(pos.y - cursor.y)
	};

	if (abs(diff.first) > maxDist) {
		diff.first = diff.first > 0 ? maxDist : -maxDist;
	}
	if (abs(diff.second) > maxDist) {
		diff.second = diff.second > 0 ? maxDist : -maxDist;
	}
	SendMessageA(gPointers->hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(cursor.x + diff.first, cursor.y + diff.second));
	prev = plr;
}