#pragma once

#include <future>
#include <atomic>

#include "PlayerManager.h"
#include "Pointers.h"

class AimBot {
public:
	static void aim() {
		using namespace std::chrono_literals;
		constexpr int maxDist = 20;
		static fb::ClientPlayer* prev{};

		if (prev && prev->clientSoldierEntity && !prev->clientSoldierEntity->IsValid()) {
			m_ready = false;
			std::thread([]() {
				SPDLOG_DEBUG("AimBot being freeze");
				std::this_thread::sleep_for(800ms);
				m_ready = true;
			}).detach();
			prev = nullptr;
		}
		if (!m_ready || !GetAsyncKeyState(VK_RBUTTON)) return;

		Vec2 pos;
		auto plr = gPlayerManager->GetClosetFromCrossHair(fb::BONE_Head, pos, true);
		if (!ValidPointer(plr)) return;

		Vec2 cursor{ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f};
		std::pair<short, short> diff{
			pos.x - cursor.x,
			pos.y - cursor.y
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