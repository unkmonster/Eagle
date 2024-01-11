#include "Renderer.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "Pointers.h"

std::promise<IDXGISwapChain*> Renderer::chain_pms;

Renderer::Renderer() {
	m_pDXGISwapChain = chain_pms.get_future().get();
	if (FAILED(m_pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_pD3dDevice))))
		throw std::runtime_error("Failed to m_pDXGISwapChain->GetDevice");
	
	m_pD3dDevice->GetImmediateContext(&m_pD3dDeviceContext);
	
	ID3D11Texture2D* pBackBuffer;
	if (SUCCEEDED(m_pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
		m_pD3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pD3D11RenderTargetView);
		pBackBuffer->Release();
	} else
		SPDLOG_WARN("Failed to DXGISwapChain->GetBuffer");
	

	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	assert(ImGui_ImplWin32_Init(gPointers->hwnd));
	assert(ImGui_ImplDX11_Init(m_pD3dDevice, m_pD3dDeviceContext));
}

Renderer::~Renderer() {
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::on_present() {
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/**/
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
	/**/

	// Rendering
	ImGui::Render();
	m_pD3dDeviceContext->OMSetRenderTargets(1, &m_pD3D11RenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
