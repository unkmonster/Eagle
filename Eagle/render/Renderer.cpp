#include "Renderer.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "Pointers.h"

std::promise<IDXGISwapChain*> Renderer::chain_pms;

Renderer::Renderer() {
	m_pDXGISwapChain = chain_pms.get_future().get();
	CreateRenderView();
	

	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	assert(ImGui_ImplWin32_Init(gPointers->hwnd));
	assert(ImGui_ImplDX11_Init(m_pD3dDevice, m_pD3dDeviceContext));
	//assert(ImGui_ImplDX11_CreateDeviceObjects());

	m_gui = Singleton<Gui>::initialize();
}

Renderer::~Renderer() {
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	this->ReleaseRenderView();
	Singleton<Gui>::destroy();
}

//void Renderer::init_dx(IDXGISwapChain * pswapchain) {
//
//}

void Renderer::on_present() {
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_gui->display();

	// Rendering
	ImGui::Render();
	m_pD3dDeviceContext->OMSetRenderTargets(1, &m_pD3D11RenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}