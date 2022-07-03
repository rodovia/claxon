#include "GraphicalOutput.h"
#include <sstream>
#include <forward_list> // std::size
#include <wrl.h>
#include <stdint.h>
#include <cmath>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#define GFX_THROW_FAILED(E) (E)
#define GFX_DEV_REMOVED(h) 0

hl2::CGraphicalOutput::CGraphicalOutput(HWND hWnd)
{
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
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

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0,
		nullptr, 0,
		D3D11_SDK_VERSION, &sd, 
		&m_Swap, &m_Device,
		nullptr, &m_Context
	));
	ComPtr<ID3D11Resource> backbuff = nullptr;
	GFX_THROW_FAILED(m_Swap->GetBuffer(0, __uuidof(ID3D11Resource), (void**) &backbuff));
	GFX_THROW_FAILED(m_Device->CreateRenderTargetView(
		backbuff.Get(),
		nullptr,
		&m_Target
	));
	
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	ComPtr<ID3D11DepthStencilState> dsstate;
	m_Device->CreateDepthStencilState(&desc, &dsstate);

	m_Context->OMSetDepthStencilState(dsstate.Get(), 1u);
	
	ComPtr<ID3D11Texture2D> depthstn;
	D3D11_TEXTURE2D_DESC descstn = {};
	descstn.Width = 800u;
	descstn.Height = 600u;
	descstn.MipLevels = 1u;
	descstn.ArraySize = 1u;
	descstn.Format = DXGI_FORMAT_D32_FLOAT;
	descstn.SampleDesc.Count = 1u;
	descstn.SampleDesc.Quality = 0u;
	descstn.Usage = D3D11_USAGE_DEFAULT;
	descstn.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	m_Device->CreateTexture2D(&descstn, nullptr, &depthstn);

	D3D11_DEPTH_STENCIL_VIEW_DESC ddsv = {};
	ddsv.Format = DXGI_FORMAT_D32_FLOAT;
	ddsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	ddsv.Texture2D.MipSlice = 0u;
	m_Device->CreateDepthStencilView(depthstn.Get(), &ddsv, &m_DepthStencil);
	m_Context->OMSetRenderTargets(1u, m_Target.GetAddressOf(), m_DepthStencil.Get());
}

void hl2::CGraphicalOutput::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = m_Swap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEV_REMOVED(m_Device->GetDeviceRemovedReason());
		}
		GFX_THROW_FAILED(hr);
	}
}

void hl2::CGraphicalOutput::ClearBuffer(float r, float g, float b) noexcept
{
	const float colors[] = { r, g, b, 1.0f };
	m_Context->ClearRenderTargetView(m_Target.Get(), colors);
	m_Context->ClearDepthStencilView(m_DepthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void hl2::CGraphicalOutput::DrawTestTriangle(float angle, float x, float y)
{
	HRESULT hr;
	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;		
	};

	const Vertex vertices[] =
	{
		{ -1.0f, -1.0f, -1.0f	},
		{ 1.0f, -1.0f, -1.0f	},
		{ -1.0f, 1.0f, -1.0f	},
		{  1.0f, 1.0f, -1.0f	},
		{ -1.0f, -1.0f, 1.0f	},
		{ 1.0f, -1.0f, 1.0f		},
		{ -1.0f, 1.0f, 1.0f		},
		{ 1.0f, 1.0f, 1.0f		}
	};

	D3D11_BUFFER_DESC dc = {};
	D3D11_SUBRESOURCE_DATA sr = {};

	sr.pSysMem = vertices;

	dc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dc.Usage = D3D11_USAGE_DEFAULT;
	dc.CPUAccessFlags = 0u;
	dc.MiscFlags = 0u;
	dc.ByteWidth = sizeof(vertices);
	dc.StructureByteStride = sizeof(Vertex);

	ComPtr<ID3D11Buffer> vb;
	GFX_THROW_FAILED(m_Device->CreateBuffer(&dc, &sr, &vb));

	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	m_Context->IASetVertexBuffers(0u, 1u, vb.GetAddressOf(), &stride, &offset);

	const unsigned short indices[] =
	{
		0, 2, 1,	2, 3, 1,
		1, 3, 5,	3, 7, 5,
		2, 6, 3,	3, 6, 7,
		4, 5, 7,	4, 7, 6,
		0, 4, 2,	2, 4, 6,
		0, 1, 4,	1, 5, 4
	};
	ComPtr<ID3D11Buffer> indbuffer;
	D3D11_BUFFER_DESC ib = {};
	ib.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ib.Usage = D3D11_USAGE_DEFAULT;
	ib.CPUAccessFlags = 0u;
	ib.MiscFlags = 0u;
	ib.ByteWidth = sizeof(indices);
	ib.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	m_Device->CreateBuffer(&ib, &isd, &indbuffer);
	m_Context->IASetIndexBuffer(indbuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};
	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				// DirectX::XMMatrixRotationZ(angle) *
				DirectX::XMMatrixRotationX(angle) *
				DirectX::XMMatrixTranslation(x, y, 4.0f) *
				DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
			)
		}
	};

	ComPtr<ID3D11Buffer> cbuf;
	D3D11_BUFFER_DESC bfd;
	bfd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bfd.Usage = D3D11_USAGE_DYNAMIC;
	bfd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bfd.MiscFlags = 0u;
	bfd.ByteWidth = sizeof(cb);
	bfd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	m_Device->CreateBuffer(&bfd, &csd, &cbuf);
	m_Context->VSSetConstantBuffers(0u, 1u, cbuf.GetAddressOf());

	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} facecolors[6];
	};

	const ConstantBuffer2 cb2 =
	{
		{
			{ 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 1.0f }
		}
	};
	ComPtr<ID3D11Buffer> cbuf2;
	D3D11_BUFFER_DESC bfd2;
	bfd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bfd2.Usage = D3D11_USAGE_DEFAULT;
	bfd2.CPUAccessFlags = 0u;
	bfd2.MiscFlags = 0u;
	bfd2.ByteWidth = sizeof(cb2);
	bfd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	m_Device->CreateBuffer(&bfd2, &csd2, &cbuf2);
	m_Context->PSSetConstantBuffers(0u, 1u, cbuf2.GetAddressOf());


	// create pixel shader
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3D11PixelShader> ps;
	D3DReadFileToBlob(L"PixelShader.cso", &blob);
	m_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ps);
	m_Context->PSSetShader(ps.Get(), nullptr, 0u);

	// create vertex shader
	ComPtr<ID3D11VertexShader> vs;
	D3DReadFileToBlob(L"VertexShader.cso", &blob);
	m_Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vs);
	m_Context->VSSetShader(vs.Get(), nullptr, 0);

	// create input (vertex) layout (2D pos only)
	ComPtr<ID3D11InputLayout> il;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_Device->CreateInputLayout(
		ied, std::size(ied),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&il
	);
	m_Context->IASetInputLayout(il.Get());

	m_Context->OMSetRenderTargets(1, m_Target.GetAddressOf(), nullptr);

	// set primitive topology to triangle
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_Context->RSSetViewports(1u, &vp);
	
	m_Context->Draw(std::size(vertices), 0u);
}

/// ^^^^ hl2::CGraphicalOutput //// vvvv hl2::CGraphicsException

hl2::CGraphicsException::CGraphicsException(int line, const char* file, HRESULT hr) noexcept
	: m_Hr(hr),
	  CBaseException(line, file)
{
}

const char* hl2::CGraphicsException::GetType() const noexcept
{
	return "Graphics Error";
}

HRESULT hl2::CGraphicsException::GetErrorCode() const noexcept
{
	return m_Hr;
}

std::string hl2::CGraphicsException::GetErrorString()
{
	std::ostringstream oss;
	oss << "[Description] " << hl2::CWindowException::TranslateErrorCode(m_Hr) << "\n"
		<< hl2::CBaseException::GetOriginString();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

std::string hl2::CGraphicsException::GetErrorDescription() const noexcept
{
	return hl2::CWindowException::TranslateErrorCode(m_Hr);
}

const char* hl2::CDeviceRemovedException::GetType() const noexcept
{
	return "Device removed";
}

const char* hl2::CNoGraphicsException::GetType() const noexcept
{
	return "No graphics";
}
