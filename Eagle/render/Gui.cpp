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
	ImGui::Begin(PROJECTNAME);
	ImGui::Checkbox("Box", &global.m_setting.m_esp.m_showBox);
	ImGui::SameLine();
	ImGui::ColorEdit4("##Box", (float*)&global.m_setting.m_esp.m_boxColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.f);
	ImGui::Combo("##box", (int*)&global.m_setting.m_esp.m_boxType, "Full\0Corner\0");

	ImGui::Checkbox("name", &global.m_setting.m_esp.m_showName);
	ImGui::End();
}

void Gui::DrawDebugMenu() {
	ImGui::Begin("DEBUG");
	if (ImGui::Button("Cls"))
		system("cls");

	if (ImGui::Button("Print Players")) {
		for (int i = 0; i < 64; ++i) {
			auto pPlayer = GetPlayerById(i);
			if (!pPlayer) continue;
			spdlog::debug("****************************************");
			spdlog::debug("[{}] '{}' teamId = {}", i, pPlayer->name, pPlayer->teamId);
			if (pPlayer->clientSoldierEntity) {
				auto soldier = pPlayer->clientSoldierEntity;
				spdlog::debug("Soldier = 0x{:X}", (uintptr_t)soldier);
				spdlog::debug("{}/{} v = {}", soldier->healthcomponent->m_Health, soldier->healthcomponent->m_MaxHealth, soldier->healthcomponent->m_VehicleHealth);
				spdlog::debug("pos = {}, {}, {}", soldier->location.x, soldier->location.y, soldier->location.z);
				spdlog::debug("{}", pPlayer->InVehicle());
			}

			if (auto vehicle = pPlayer->clientVehicleEntity; ValidPointer(vehicle)) {
				spdlog::debug("Vehicle = 0x{:X}", (uintptr_t)vehicle);
				auto pos = vehicle->GetVehiclePosition();
				spdlog::debug("({}, {}, {})", pos.x, pos.y, pos.z);
			}	
		}
	}

	if (ImGui::Button("Unload")) 
		std::thread(FreeLibrary, global.m_thisModule).detach();
	
	ImGui::End();
}
