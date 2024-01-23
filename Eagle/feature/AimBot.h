#pragma once

#include <future>
#include <atomic>

#include "PlayerManager.h"
#include "Pointers.h"

class AimBot {
public:
	static void run() {
		using namespace std::chrono_literals;
		constexpr int maxDist = 20;
		static fb::ClientPlayer* prev{};

		if (prev && prev->clientSoldierEntity && !prev->clientSoldierEntity->IsValid()) {
			m_ready = false;
			std::thread([]() {
				std::this_thread::sleep_for(800ms);
				m_ready = true;
			}).detach();
			prev = nullptr;
		}
		if (!m_ready || !GetAsyncKeyState(VK_RBUTTON)) return;

		Vec2 pos;
		auto plr = gPlayerManager->GetClosetFromCrossHair(fb::BONE_Head, pos, false);
		if (!ValidPointer(plr)) return;
		Vec2 cursor{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
		
		auto min = pos - Vec2{global.m_setting.m_aimBot.m_areaSize.x / 2.f, global.m_setting.m_aimBot.m_areaSize.y / 2.f};
		auto max = min + global.m_setting.m_aimBot.m_areaSize;
		if (!InBound(pos, min, max)) return;

		if (gPlayerManager->m_localPlayer) {
			auto selfPos = gPlayerManager->m_localPlayer->GetPosition();
			auto targetPos = plr->GetPosition();
			if (selfPos && targetPos && distance(*selfPos, *targetPos) > global.m_setting.m_aimBot.m_maxDistance) 
				return;
		}
		
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

	static std::atomic_bool m_ready;
};
inline std::atomic_bool AimBot::m_ready = true;