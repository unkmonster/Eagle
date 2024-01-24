#pragma once

#include <vector>
#include <functional>
#include <map>
#include <string>

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui.h>

#include "utils/Singleton.h"
#include "Pointers.h"
#include "FileManager.h"

class Gui {
	friend Singleton<Gui>;
	Gui(ID3D11Device* pD3dDevice, ID3D11DeviceContext* pD3dDeviceContext);
public:
	~Gui() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		FreeLibrary(m_langModule);
	}

	void DrawMenu();
	void DrawObserversMenu(std::vector<char*>);
	static void DrawDebugMenu(std::vector<std::string>);
private:
	std::vector<std::pair<std::string, std::function<void(Gui*)>>> m_pages;
	ImFont* m_menuFont;
	HMODULE m_langModule;

	static void SetImGuiTheme();
	static void BigDarkTheme();
	
	void MenuBar();

	void EspPage();
	void AimBotPage();
	void MiscPage();
};

