#pragma once

#include <d3d11.h>
#include <future>

#include <imgui.h>

#include "Logger.h"
#include "utils/Singleton.h"


class Renderer {
	friend Singleton<Renderer>;
	friend class PlayerManager;
	Renderer();
public:
	~Renderer();

	void on_present();

	void CreateRenderView(IDXGISwapChain* pSwapChain) {
		// device
		if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_pD3dDevice))))
			throw std::runtime_error("Failed to pSwapChain->GetDevice");

		m_pD3dDevice->GetImmediateContext(&m_pD3dDeviceContext);		// context

		// render target view
		ID3D11Texture2D* pBackBuffer;
		if (SUCCEEDED(pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
			m_pD3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pD3D11RenderTargetView);
			pBackBuffer->Release();
		} else
			SPDLOG_WARN("Failed to DXGISwapChain->GetBuffer");
	}

	void ReleaseRenderView() {
		if (m_pD3D11RenderTargetView) {
			m_pD3D11RenderTargetView->Release();
			m_pD3D11RenderTargetView = nullptr;
		}
	}
private:
	ID3D11Device* m_pD3dDevice{};
	ID3D11DeviceContext* m_pD3dDeviceContext{};
	ID3D11RenderTargetView* m_pD3D11RenderTargetView{};
	float m_fontSize = 14.f;
	ImFont* m_textFont;
	ImFont* m_menuFont;
};

inline Renderer* gRenderer{};