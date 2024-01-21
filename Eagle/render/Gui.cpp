#include "Gui.h"

#include <Windows.h>
#include <string>
#include <thread>

#include <imgui.h>
#include <fmt/format.h>
#include "bf-1/Frosbite.h"
#include "bf-1/Obfuscationmgr.h"

#include "Logger.h"
#include "Global.h"
#include "feature/PlayerManager.h"

void Gui::DrawMenu() {
	ImGui::Begin(PROJECTNAME, nullptr, ImGuiWindowFlags_MenuBar);
	MenuBar();

	static int selected = 0;
	if (ImGui::BeginChild("index", {ImGui::GetWindowSize().x * 0.2f, 0})) {
		for (int i = 0; i < m_pages.size(); ++i) {
			if (ImGui::Selectable(m_pages[i].first.c_str(), selected == i))
				selected = i;
		}
		ImGui::EndChild();
	}

	ImGui::SameLine();
	m_pages[selected].second();

	ImGui::End();
}

void Gui::DrawDebugMenu(std::vector<Vec2> positions) {
	ImGui::Begin("position");
	if (ImGui::BeginTable("position", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		for (int i = 0; i < positions.size(); ++i) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(fmt::format("({}, {})", (int)positions[i].x, (int)positions[i].y).c_str());
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

void Gui::MenuBar() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::MenuItem("Unload"))
				std::thread(FreeLibrary, global.m_thisModule).detach();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void Gui::EspPage() {
	auto& espSet = global.m_setting.m_esp;
	ImGui::BeginChild("EspPage");
	ImGui::Checkbox("Box", &espSet.m_showBox);
	
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75.f);
	ImGui::Combo("##Type", (int*)&espSet.m_boxType, "Full\0Corner\0Cube\0");
	
	ImGui::SameLine();
	ImGui::ColorEdit4("##box color", (float*)&espSet.m_boxColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	ImGui::ColorEdit4("Visible/Invisible", (float*)&espSet.m_boxColorOccluded, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	ImGui::ColorEdit4("Vehicle", (float*)&espSet.m_vehicleColor, ImGuiColorEditFlags_NoInputs);
	
	
	ImGui::Checkbox("Status Line", &espSet.m_showStatusLine);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75.f);
	ImGui::Combo("##Status Line Type", (int*)&espSet.m_statusLinePos, "Up\0Bottom\0");

	ImGui::Checkbox("Health Bar", &espSet.m_showHealthBar);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75.f);
	ImGui::Combo("##Health Bar type", (int*)&espSet.m_healthBarPos, "Top\0Bottom\0Left\0Right\0");

	ImGui::SeparatorText("Text Information");
	ImGui::Checkbox("name", &espSet.m_showName);
	ImGui::SameLine();
	ImGui::Checkbox("Health", &espSet.m_showHp);
	ImGui::SameLine();
	ImGui::Checkbox("Distance", &espSet.m_showDistance);

	ImGui::SeparatorText("CrossHair");
	ImGui::Checkbox("Show", &espSet.m_showCrossHair);
	ImGui::SameLine();
	ImGui::ColorEdit4("##", (float*)&espSet.m_crossHairColor, ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	ImGui::ColorEdit4("##", (float*)&espSet.m_crossHairColorAtEnemy, ImGuiColorEditFlags_NoInputs);

	ImGui::Separator();
	ImGui::SliderInt("Effective Distance", &espSet.m_effective, 0, 3000);
	ImGui::EndChild();
}

void Gui::AimBotPage() {
	if (ImGui::BeginChild("AimBotPage")) {
		auto& aimbot_set = global.m_setting.m_aimBot;

		ImGui::Checkbox("Enable", &aimbot_set.m_enable);
		ImGui::EndChild();
	}
}

void Gui::MiscPage() {
	if (ImGui::BeginChild("MiscPage")) {
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputFloat("Global Font Size", &global.m_setting.m_textSize, 1.f);
		ImGui::EndChild();
	}
}
