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

void Gui::DrawObserversMenu(std::vector<char*> observers) {
	if (observers.empty()) return;

	ImGui::Begin("Observers", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::BeginTable("position", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		for (int i = 0; i < observers.size(); ++i) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(observers[i]? observers[i] : "unknown");
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

void Gui::DrawDebugMenu(std::vector<std::string> texts) {
	ImGui::Begin("Dbg", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::BeginTable("Dbg", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		for (int i = 0; i < texts.size(); ++i) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(texts[i].c_str());
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
	static auto& size = ImGui::GetIO().DisplaySize;

	if (ImGui::BeginChild("AimBotPage")) {
		auto& aimbot_set = global.m_setting.m_aimBot;

		ImGui::Checkbox("Enable", &aimbot_set.m_enable);
		
		ImGui::Checkbox("Show Area", &aimbot_set.m_showArea);
		ImGui::SameLine();
		ImGui::ColorEdit4("##", (float*)&aimbot_set.m_areaColor, ImGuiColorEditFlags_NoInputs);

		ImGui::SliderFloat("X", &aimbot_set.m_areaSize.x, 0, size.x);
		ImGui::SliderFloat("Y", &aimbot_set.m_areaSize.y, 0, size.y);
		ImGui::SliderInt("Max Effective Distance", &aimbot_set.m_maxDistance, 0, 3000);
		ImGui::Checkbox("Only Visible", &aimbot_set.m_onlyVisible);
		ImGui::InputInt("CD (ms)", &aimbot_set.m_coldDown, 50, 100, ImGuiInputTextFlags_None);
		static const char* bones[] = {"Head", "Neck", "Spine2", "Spine1", "Spine"};
		ImGui::SliderInt("Pos", &aimbot_set.m_bone, 0, 4, (aimbot_set.m_bone >= 0 && aimbot_set.m_bone < 5)? bones[aimbot_set.m_bone] : "unk", ImGuiSliderFlags_NoInput);
		ImGui::EndChild();
	}
}

void Gui::MiscPage() {
	if (ImGui::BeginChild("MiscPage")) {
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputFloat("Global Font Size", &global.m_setting.m_textSize, 1.f);
		ImGui::Checkbox("Show Observers", &global.m_setting.m_showObserversList);
		ImGui::EndChild();
	}
}
