#pragma once

#include <d3d11.h>
#include <future>

#include "Logger.h"
#include "utils/Singleton.h"
#include "Gui.h"

class Renderer {
	friend Singleton<Renderer>;
	Renderer();
public:
	~Renderer();
	static std::promise<IDXGISwapChain*> chain_pms;

	//void init_dx(IDXGISwapChain* pswapchain);

	void on_present();

	void CreateRenderView() {
		// device
		if (FAILED(m_pDXGISwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_pD3dDevice))))
			throw std::runtime_error("Failed to m_pDXGISwapChain->GetDevice");

		m_pD3dDevice->GetImmediateContext(&m_pD3dDeviceContext);		// context

		// render target view
		ID3D11Texture2D* pBackBuffer;
		if (SUCCEEDED(m_pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
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
	IDXGISwapChain* m_pDXGISwapChain{};
	ID3D11Device* m_pD3dDevice{};
	ID3D11DeviceContext* m_pD3dDeviceContext{};
	ID3D11RenderTargetView* m_pD3D11RenderTargetView{};
	Gui* m_gui;
};

inline Renderer* gRenderer{};