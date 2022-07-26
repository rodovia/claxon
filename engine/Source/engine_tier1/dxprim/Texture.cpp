#include "Texture.h"
#include <engine_tier1/Surface.h>
#include <engine_tier0/Exceptions.h>

engine::CTexture::CTexture(engine::CGraphicalOutput& _Gfx,
							const engine::CSurface& _Surfc, unsigned int _Slot)
	: m_Slot(_Slot)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = _Surfc.GetWidth();
	desc.Height = _Surfc.GetHeight();
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = _Surfc.GetBufferPointer();
	sd.SysMemPitch = _Surfc.GetWidth() * sizeof(engine::CColor);
	CUtl_ComPtr<ID3D11Texture2D> tex;
	_ENGINE_MAYTHROW_GRAPHICS(
		GetDevice(_Gfx)->CreateTexture2D(
			&desc, &sd, &tex
		)
	);

	D3D11_SHADER_RESOURCE_VIEW_DESC vd = {};
	vd.Format = desc.Format;
	vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	vd.Texture2D.MostDetailedMip = 0;
	vd.Texture2D.MipLevels = 1;
	_ENGINE_MAYTHROW_GRAPHICS(
		GetDevice(_Gfx)->CreateShaderResourceView(
			tex.Get(), &vd, &m_TextureView
		)
	);
}

void engine::CTexture::Bind(engine::CGraphicalOutput& _Gfx) noexcept
{
	GetContext(_Gfx)->PSSetShaderResources(m_Slot, 1u, m_TextureView.GetAddressOf());
}
