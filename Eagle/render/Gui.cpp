#include "Gui.h"

#include <Windows.h>
#include <string>
#include <thread>
#include <d3d11.h>

#include <fmt/format.h>
#include "bf-1/Frosbite.h"
#include "bf-1/Obfuscationmgr.h"

#include "Logger.h"
#include "Global.h"
#include "feature/PlayerManager.h"
#include "../en-US/resource.h"

Gui::Gui(ID3D11Device* pD3dDevice, ID3D11DeviceContext* pD3dDeviceContext) {
	m_langModule = LoadLibraryEx((gFileManager->m_base / "lang.dll").c_str(), 
		NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
	if (!m_langModule)
		throw std::runtime_error("Failed to load 'lang.dll'");

	ImGui::CreateContext();
	auto & io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.MouseDrawCursor = true;
	SetImGuiTheme();

	// ¼ÓÔØ×ÖÌå
	auto langId = GetThreadLocale();
	SPDLOG_DEBUG("LANGID: 0x{:X}", langId);
	auto fontSize = global.m_setting.m_menuFontSize;
	if (langId == 0x0804)
		m_menuFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\msyh.ttc)", 
			fontSize, 0, io.Fonts->GetGlyphRangesChineseFull());
	else
		m_menuFont = io.Fonts->AddFontFromFileTTF((gFileManager->m_base / "Roboto-Medium.ttf").string().c_str(), fontSize);

	// Setup Platform/Renderer backends
	assert(ImGui_ImplWin32_Init(gPointers->hwnd));
	assert(ImGui_ImplDX11_Init(pD3dDevice, pD3dDeviceContext));

	m_pages.emplace_back(LoadStrU8(m_langModule, STR_ESP).c_str(), &Gui::EspPage);
	m_pages.emplace_back(LoadStrU8(m_langModule, STR_AIMBOT).c_str(), &Gui::AimBotPage);
	m_pages.emplace_back(LoadStrU8(m_langModule, STR_MISC).c_str(), &Gui::MiscPage);
}

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
	m_pages[selected].second(this);

	ImGui::End();
}

void Gui::DrawObserversMenu(std::vector<char*> observers) {
	if (observers.empty()) return;

	ImGui::Begin(LoadStrU8(m_langModule, STR_OBSERVERS).c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize);
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

void Gui::SetImGuiTheme() {
	switch (global.m_setting.m_theme) {
	case 0:
		ImGui::StyleColorsDark(); break;
	case 1:
		ImGui::StyleColorsLight(); break;
	case 2:
		ImGui::StyleColorsClassic(); break;
	case 3:
		BigDarkTheme(); break;
	}
}

void Gui::BigDarkTheme() {
	auto &style = ImGui::GetStyle();
	style.WindowPadding = {10.f, 10.f};
	style.PopupRounding = 0.f;
	style.FramePadding = {8.f, 4.f};
	style.ItemSpacing = {10.f, 8.f};
	style.ItemInnerSpacing = {6.f, 6.f};
	style.TouchExtraPadding = {0.f, 0.f};
	style.IndentSpacing = 21.f;
	style.ScrollbarSize = 15.f;
	style.GrabMinSize = 8.f;
	style.WindowBorderSize = 1.f;
	style.ChildBorderSize = 0.f;
	style.PopupBorderSize = 1.f;
	style.FrameBorderSize = 0.f;
	style.TabBorderSize = 0.f;
	style.WindowRounding = 0.f;
	style.ChildRounding = 0.f;
	style.FrameRounding = 0.f;
	style.ScrollbarRounding = 0.f;
	style.GrabRounding = 0.f;
	style.TabRounding = 0.f;
	style.WindowTitleAlign = {0.5f, 0.5f};
	style.ButtonTextAlign = {0.5f, 0.5f};
	style.DisplaySafeAreaPadding = {3.f, 3.f};

	auto &colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.90f, 0.19f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.27f, 0.27f, 0.54f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.39f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.87f);
	colors[ImGuiCol_Header] = ImVec4(0.37f, 0.37f, 0.37f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.38f, 0.38f, 0.37f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.37f, 0.37f, 0.37f, 0.51f);
	colors[ImGuiCol_Separator] = ImVec4(0.38f, 0.38f, 0.38f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.50f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.64f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 0.86f);
	colors[ImGuiCol_TabActive] = ImVec4(0.34f, 0.34f, 0.34f, 0.86f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void Gui::MenuBar() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(LoadStrU8(m_langModule, STR_MENU).c_str())) {
			if (ImGui::MenuItem(LoadStrU8(m_langModule, STR_UNLOAD).c_str()))
				std::thread(FreeLibrary, global.m_thisModule).detach();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void Gui::EspPage() {
	auto& espSet = global.m_setting.m_esp;
	ImGui::BeginChild("EspPage");
	ImGui::Checkbox(LoadStrU8(m_langModule, STR_BOX).c_str(), &espSet.m_showBox);
	
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75.f);
	ImGui::Combo("##Type", (int*)&espSet.m_boxType, "Rect\0Corner\0Cube\0");
	
	ImGui::SameLine();
	ImGui::ColorEdit4("##box color", (float*)&espSet.m_boxColor, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	ImGui::ColorEdit4(LoadStrU8(m_langModule, STR_VISIBLE_INVISIBLE).c_str(), (float*)&espSet.m_boxColorOccluded, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	ImGui::ColorEdit4(LoadStrU8(m_langModule, STR_VEHICLE).c_str(), (float*)&espSet.m_vehicleColor, ImGuiColorEditFlags_NoInputs);
	
	
	ImGui::Checkbox(LoadStrU8(m_langModule, STR_STATUS_LINE).c_str(), &espSet.m_showStatusLine);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75.f);
	ImGui::Combo("##Status Line Type", (int*)&espSet.m_statusLinePos, "Up\0Bottom\0");

	ImGui::Checkbox(LoadStrU8(m_langModule, STR_HEALTH_BAR).c_str(), &espSet.m_showHealthBar);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75.f);
	ImGui::Combo("##Health Bar type", (int*)&espSet.m_healthBarPos, "Top\0Bottom\0Left\0Right\0");

	ImGui::SeparatorText(LoadStrU8(m_langModule, STR_TEXT_INFORMATION).c_str());
	ImGui::Checkbox(LoadStrU8(m_langModule, STR_NAME).c_str(), &espSet.m_showName);
	ImGui::SameLine();
	ImGui::Checkbox(LoadStrU8(m_langModule, STR_HEALTH).c_str(), &espSet.m_showHp);
	ImGui::SameLine();
	ImGui::Checkbox(LoadStrU8(m_langModule, STR_DISTANCE).c_str(), &espSet.m_showDistance);

	ImGui::SeparatorText(LoadStrU8(m_langModule, STR_CROSSHAIR).c_str());
	ImGui::Checkbox(LoadStrU8(m_langModule, STR_SHOW).c_str(), &espSet.m_showCrossHair);
	ImGui::SameLine();
	ImGui::ColorEdit4("##", (float*)&espSet.m_crossHairColor, ImGuiColorEditFlags_NoInputs);
	ImGui::SameLine();
	ImGui::ColorEdit4("##", (float*)&espSet.m_crossHairColorAtEnemy, ImGuiColorEditFlags_NoInputs);

	ImGui::Separator();
	ImGui::SliderInt(LoadStrU8(m_langModule, STR_EFFECTIVE_DISTANCE).c_str(), &espSet.m_effective, 0, 3000);
	ImGui::EndChild();
}

void Gui::AimBotPage() {
	static auto& size = ImGui::GetIO().DisplaySize;

	if (ImGui::BeginChild("AimBotPage")) {
		auto& aimbot_set = global.m_setting.m_aimBot;

		ImGui::Checkbox(LoadStrU8(m_langModule, STR_ENABLE).c_str(), &aimbot_set.m_enable);
		
		ImGui::Checkbox(LoadStrU8(m_langModule, STR_SHOW_AREA).c_str(), &aimbot_set.m_showArea);
		ImGui::SameLine();
		ImGui::ColorEdit4("##", (float*)&aimbot_set.m_areaColor, ImGuiColorEditFlags_NoInputs);

		ImGui::SliderFloat("X", &aimbot_set.m_areaSize.x, 0, size.x);
		ImGui::SliderFloat("Y", &aimbot_set.m_areaSize.y, 0, size.y);
		ImGui::SliderInt(LoadStrU8(m_langModule, STR_MAX_EFFECTIVE_DISTANCE).c_str(), &aimbot_set.m_maxDistance, 0, 3000);
		ImGui::Checkbox(LoadStrU8(m_langModule, STR_ONLY_VISIBLE).c_str(), &aimbot_set.m_onlyVisible);
		ImGui::InputInt("CD (ms)", &aimbot_set.m_coldDown, 50, 100, ImGuiInputTextFlags_None);
		static const std::string bones[] = {
			LoadStrU8(m_langModule, STR_HEAD), LoadStrU8(m_langModule, STR_NECK), 
			LoadStrU8(m_langModule, STR_SPINE) + "2", LoadStrU8(m_langModule, STR_SPINE) + "1",
			LoadStrU8(m_langModule, STR_SPINE)
		};
		ImGui::SliderInt(LoadStrU8(m_langModule, STR_POSITION).c_str(), &aimbot_set.m_bone, 0, 4, (aimbot_set.m_bone >= 0 && aimbot_set.m_bone < 5)? bones[aimbot_set.m_bone].c_str() : "unk", ImGuiSliderFlags_NoInput);
		ImGui::EndChild();
	}
}

void Gui::MiscPage() {
	if (ImGui::BeginChild("MiscPage")) {
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputFloat(LoadStrU8(m_langModule, STR_GLOBAL_FONT_SIZE).c_str(), &global.m_setting.m_textSize, 1.f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputFloat(LoadStrU8(m_langModule,	STR_MENU_FONT_SIZE).c_str(), &global.m_setting.m_menuFontSize, 1.f);

		ImGui::Checkbox(LoadStrU8(m_langModule, STR_SHOW_OBSERVERS).c_str(), &global.m_setting.m_showObserversList);

		ImGui::SetNextItemWidth(100.f);
		if (ImGui::Combo(LoadStrU8(m_langModule, STR_THEME).c_str(), &global.m_setting.m_theme, "Dark\0Light\0Classic\0BigDark")) 
			SetImGuiTheme();
		ImGui::EndChild();
	}
}