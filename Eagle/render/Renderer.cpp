#include "Renderer.h"


#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "Gui.h"
#include "Pointers.h"
#include "feature/PlayerManager.h"
#include "FileManager.h"

Renderer::Renderer() {
	CreateRenderView(gPointers->pSwapChain);
	
	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	m_menuFont = ImGui::GetIO().Fonts->AddFontFromFileTTF((gFileManager->m_base / "Roboto-Medium.ttf").string().c_str(), m_fontSize);
	m_textFont = ImGui::GetIO().Fonts->AddFontFromFileTTF((gFileManager->m_base / "Spiegel_TT_SemiBold.ttf").string().c_str(), m_fontSize);

	// Setup Platform/Renderer backends
	assert(ImGui_ImplWin32_Init(gPointers->hwnd));
	assert(ImGui_ImplDX11_Init(m_pD3dDevice, m_pD3dDeviceContext));
	//assert(ImGui_ImplDX11_CreateDeviceObjects());
}

Renderer::~Renderer() {
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ReleaseRenderView();
}

void Renderer::on_present() {
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (global.m_showMenu) {
		Gui::DrawMenu();
		Gui::DrawDebugMenu();
	}

	gPlayerManager->run();

	// Rendering
	ImGui::Render();
	m_pD3dDeviceContext->OMSetRenderTargets(1, &m_pD3D11RenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
