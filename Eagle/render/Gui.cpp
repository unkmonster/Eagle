#include "Gui.h"

#include <Windows.h>
#include <string>

#include <imgui.h>
#include <fmt/format.h>
#include "bf-1/Frosbite.h"
#include "bf-1/Obfuscationmgr.h"

#include "Logger.h"
#include "Global.h"

void Gui::DrawMenu() {
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
}

void Gui::DrawDebugMenu() {
	ImGui::Begin("DEBUG");
	if (ImGui::Button("Cls"))
		system("cls");

	if (ImGui::Button("Print Players")) {
		for (int i = 0; i < 64; ++i) {
			auto pPlayer = GetPlayerById(i);
			if (!pPlayer) continue;
			spdlog::debug("******************************");
			spdlog::debug("[{}] '{}' teamId = {}", i, pPlayer->name, pPlayer->teamId);
			if (!pPlayer->clientSoldierEntity) continue;
			auto soldier = pPlayer->clientSoldierEntity;
			spdlog::debug("Soldier = 0x{:X}", (uintptr_t)soldier);
			spdlog::debug("{}/{} v = {}", soldier->healthcomponent->m_Health, soldier->healthcomponent->m_MaxHealth, soldier->healthcomponent->m_VehicleHealth);
			spdlog::debug("pos = {}, {}, {}", soldier->location.x, soldier->location.y, soldier->location.z);
		}
	}

	if (ImGui::Button("Unload")) 
		std::thread(FreeLibrary, global.m_thisModule).detach();
	
	ImGui::End();
}
