#pragma once

#include <d3d11.h>
#include <future>

#include "utils/Singleton.h"

class Renderer {
	friend Singleton<Renderer>;
	Renderer();
public:
	~Renderer();
	static std::promise<IDXGISwapChain*> chain_pms;

	void on_present();
private:
	IDXGISwapChain* m_pDXGISwapChain{};
	ID3D11Device* m_pD3dDevice{};
	ID3D11DeviceContext* m_pD3dDeviceContext{};
	ID3D11RenderTargetView* m_pD3D11RenderTargetView{};
};

inline Renderer* gRenderer{};