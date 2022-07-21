#include "GraphicalOutput.h"
#include <DirectXMath.h>
#include <forward_list> // std::size
#include <tier0lib/dxerr.h>

#include <imguihlp.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <engine_tier0/Exceptions.h>

engine::CGraphicalOutput::CGraphicalOutput(HWND hWnd, ImGuiContext* _Ctx)
{
	HRESULT hr;

	if (_Ctx == nullptr)
	{
		m_HasImGui = false;
	}
	else
	{
		engine::imgui::SetCurrentContext(_Ctx);
		engine::imgui::SetAllocProcs();
	}

	RECT wr;
	GetWindowRect(hWnd, &wr);
	int width = wr.right;
	int height = wr.bottom;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// TODO: Adicionar um argumento de linha de comando 
	//	para que D3D11CreateDeviceAndSwapChain possa criar um dispositivo
	//	D3D_FEATURE_LEVEL_11_1.
	// CONTEXTO AO CIDADÃO: Por algum motivo, se especificar D3D_FEATURE_LEVEL_11_1
	// e o driver não for compatível com o nível, a função irá falhar. Independente
	// se fLevels especificar algum nível após o 11_1.
	const D3D_FEATURE_LEVEL fLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};

	_ENGINE_MAYTHROW_GRAPHICS(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		fLevels,
		std::size(fLevels),
		D3D11_SDK_VERSION,
		&sd,
		&m_Swap,
		&m_Device,
		&m_FeatLevel,
		&m_Context
	));
	ID3D11Resource* backbuf;
	m_Swap->GetBuffer(0, __uuidof(ID3D11Resource), (void**) &backbuf);
	m_Device->CreateRenderTargetView(backbuf, nullptr, &m_Target);

	D3D11_DEPTH_STENCIL_DESC deds;
	ZeroMemory(&deds, sizeof(D3D11_DEPTH_STENCIL_DESC));
	deds.DepthEnable = TRUE;
	deds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	deds.DepthFunc = D3D11_COMPARISON_LESS;
	ID3D11DepthStencilState* dstate;
	_ENGINE_MAYTHROW_GRAPHICS(m_Device->CreateDepthStencilState(&deds, &dstate));

	m_Context->OMSetDepthStencilState(dstate, 0u);

	ID3D11Texture2D* dstx;
	D3D11_TEXTURE2D_DESC descdepth;
	ZeroMemory(&descdepth, sizeof(D3D11_TEXTURE2D_DESC));
	// TODO: trocar as resoluções hardcoded
	descdepth.Width = width;
	descdepth.Height = height;
	descdepth.MipLevels = 1u;
	descdepth.ArraySize = 1u;
	descdepth.Format = DXGI_FORMAT_D32_FLOAT;
	descdepth.SampleDesc.Count = 1u;
	descdepth.SampleDesc.Quality = 0u;
	descdepth.Usage = D3D11_USAGE_DEFAULT;
	descdepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	m_Device->CreateTexture2D(&descdepth, nullptr, &dstx);

	D3D11_DEPTH_STENCIL_VIEW_DESC ddsv;
	ZeroMemory(&ddsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ddsv.Format = DXGI_FORMAT_D32_FLOAT;
	ddsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ddsv.Texture2D.MipSlice = 0u;
	_ENGINE_MAYTHROW_GRAPHICS(m_Device->CreateDepthStencilView(dstx, &ddsv, &m_DepthStencil));

	m_Context->OMSetRenderTargets(1u, m_Target.GetAddressOf(), m_DepthStencil.Get());

	D3D11_VIEWPORT vp;
	// TODO: trocar as resoluções hardcoded
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	m_Context->RSSetViewports(1u, &vp);

	ImGui_ImplDX11_Init(m_Device.Get(), m_Context.Get());
}

engine::CGraphicalOutput::~CGraphicalOutput()
{
	ImGui_ImplDX11_Shutdown();
}

void engine::CGraphicalOutput::ToggleImGui() noexcept
{
	m_HasImGui = !m_HasImGui;
}

bool engine::CGraphicalOutput::ImGuiEnabled() const noexcept
{
	return m_HasImGui;
}

void engine::CGraphicalOutput::BeginFrame(float _R, float _G, float _B)
{
	assert(!m_PendingEndFrameCall);

	m_PendingEndFrameCall = true;

	if (m_HasImGui)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float colors[] = { _R, _G, _B, 1.0f };
	m_Context->ClearRenderTargetView(m_Target.Get(), colors);
	m_Context->ClearDepthStencilView(m_DepthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void engine::CGraphicalOutput::BeginFrameNorm(unsigned char _R, unsigned char _G, unsigned char _B)
{
	this->BeginFrame(_R / 255, _G / 255, _B / 255);
}

void engine::CGraphicalOutput::EndFrame()
{
	assert(m_PendingEndFrameCall);
	m_PendingEndFrameCall = false;

	if (m_HasImGui)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	
	HRESULT hr;
	m_InfoQueue.Set();
	if (FAILED(hr = m_Swap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw _ENGINE_CREATE_DEVC(m_Device->GetDeviceRemovedReason());
		}
	}
}

void engine::CGraphicalOutput::DrawIndexed(UINT _Count)
{
	m_Context->DrawIndexed(_Count, 0u, 0u);
}

DirectX::XMMATRIX engine::CGraphicalOutput::GetProjection() const noexcept
{
	return m_Projection;
}

void engine::CGraphicalOutput::SetProjection(DirectX::FXMMATRIX _Proj) noexcept
{
	m_Projection = _Proj;
}

DirectX::XMMATRIX engine::CGraphicalOutput::GetCamera() const noexcept
{
	return m_Camera;
}

void engine::CGraphicalOutput::SetCamera(DirectX::FXMMATRIX _Cam) noexcept
{
	m_Camera = _Cam;
}

D3D_FEATURE_LEVEL engine::CGraphicalOutput::GetFeatureLevel() const noexcept
{
	return m_FeatLevel;
}
