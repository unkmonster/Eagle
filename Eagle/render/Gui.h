#pragma once

#include <vector>
#include <functional>
#include <map>
#include <string>

#include <d3d11.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "utils/Singleton.h"
#include "Pointers.h"
#include "FileManager.h"

class Gui {
	friend Singleton<Gui>;
	Gui(ID3D11Device* pD3dDevice, ID3D11DeviceContext* pD3dDeviceContext) {
		ImGui::CreateContext();

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		ImGui::StyleColorsDark();

		m_menuFont = ImGui::GetIO().Fonts->AddFontFromFileTTF((gFileManager->m_base / "Roboto-Medium.ttf").string().c_str(), m_fontSize);
		ImGui::GetIO().MouseDrawCursor = true;

		// Setup Platform/Renderer backends
		assert(ImGui_ImplWin32_Init(gPointers->hwnd));
		assert(ImGui_ImplDX11_Init(pD3dDevice, pD3dDeviceContext));

		m_pages.emplace_back("Esp", EspPage);
		m_pages.emplace_back("AimBot", AimBotPage);
		m_pages.emplace_back("Misc", MiscPage);
	}
public:
	~Gui() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void DrawMenu();
	void DrawDebugMenu();
private:
	std::vector<std::pair<std::string, std::function<void()>>> m_pages;
	ImFont* m_menuFont;
	float m_fontSize = 15.f;

	static void MenuBar();

	static void EspPage();
	static void AimBotPage();
	static void MiscPage();
};