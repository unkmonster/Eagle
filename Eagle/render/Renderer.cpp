#include "Renderer.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "Gui.h"
#include "Pointers.h"
#include "feature/PlayerManager.h"
#include "feature/AimBot.h"
#include "feature/Esp.h"
#include "FileManager.h"

Renderer::Renderer() {
	// Anti-ScreenShot
	if (!SetWindowDisplayAffinity(gPointers->hwnd, WDA_EXCLUDEFROMCAPTURE))
		SPDLOG_WARN(GetLastErrorTextA().get());

	CreateRenderView(gPointers->pSwapChain);
	m_gui = Singleton<Gui>::initialize(m_pD3dDevice, m_pD3dDeviceContext);

	m_textFont = ImGui::GetIO().Fonts->AddFontFromFileTTF((gFileManager->m_base / "Spiegel_TT_SemiBold.ttf").string().c_str(), 16.f);
}

Renderer::~Renderer() {
	// Cleanup
	Singleton<Gui>::destroy();
	ReleaseRenderView();

	if (!SetWindowDisplayAffinity(gPointers->hwnd, WDA_NONE))
		SPDLOG_WARN(GetLastErrorTextA().get());
}

void Renderer::on_present() {
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	try {
		if (global.m_showMenu) {
			m_gui->DrawMenu();
		}

		gPlayerManager->update();
		Gui::DrawDebugMenu(gPlayerManager->debug_info());
		Esp::run();

		if (global.m_setting.m_aimBot.m_enable)
			AimBot::run();
		if (global.m_setting.m_showObserversList)
			m_gui->DrawObserversMenu(gPlayerManager->observers());
	} catch (const std::runtime_error& error) {
		MessageBoxA(NULL, error.what(), PROJECTNAME, MB_ICONWARNING);
		SendMessage(gPointers->hwnd, WM_KEYDOWN, VK_END, 0);
	}
	
	// Rendering
	ImGui::Render();
	m_pD3dDeviceContext->OMSetRenderTargets(1, &m_pD3D11RenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
