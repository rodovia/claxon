#include "Texture.h"
#include <engine_tier1/Surface.h>
#include <engine_tier0/Exceptions.h>
#include <tier0lib/String0.h>
#include <typeinfo>
#include <engine_ui/Console.h>

engine::CConVar mipmap_lvl("mipmap_lvl", "4");

engine::CTexture::CTexture(engine::CGraphicalOutput& _Gfx, const std::wstring& _Filename, unsigned int _Slot)
	: m_Slot(_Slot),
	m_Name(_Filename)
{
	const CSurface surf = engine::CSurface::FromFile(m_Name);

	m_HasAlpha = surf.HasAlpha();

	HRESULT hr;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = surf.GetWidth();
	desc.Height = surf.GetHeight();
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = surf.GetBufferPointer();
	sd.SysMemPitch = surf.GetWidth() * sizeof(engine::CColor);
	CUtl_ComPtr<ID3D11Texture2D> tex;
	_ENGINE_MAYTHROW_GRAPHICS(
		GetDevice(_Gfx)->CreateTexture2D(
			&desc, nullptr, &tex
		)
	);

	GetContext(_Gfx)->UpdateSubresource(
		tex.Get(), 0u, nullptr, surf.GetBufferPointerConst(), 
		surf.GetWidth() * sizeof(engine::CColor), 0u);

	D3D11_SHADER_RESOURCE_VIEW_DESC vd = {};
	vd.Format = desc.Format;
	vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	vd.Texture2D.MostDetailedMip = 0;
	vd.Texture2D.MipLevels = -1;
	_ENGINE_MAYTHROW_GRAPHICS(
		GetDevice(_Gfx)->CreateShaderResourceView(
			tex.Get(), &vd, &m_TextureView
		)
	);

	GetContext(_Gfx)->GenerateMips(m_TextureView.Get());
}

void engine::CTexture::Bind(engine::CGraphicalOutput& _Gfx) noexcept
{
	GetContext(_Gfx)->PSSetShaderResources(m_Slot, 1u, m_TextureView.GetAddressOf());
}

std::string engine::CTexture::Discriminate(const std::wstring& _Filename, unsigned int _Slot) noexcept
{
	using namespace std::string_literals;
	std::wstring cpy = _Filename;

	return typeid(engine::CTexture).name() + "@"s 
		+ std::to_string(_Slot) + "@"s
		+ tier0::ConvertToMultiByteString(cpy);
}

std::string engine::CTexture::GenerateDiscriminator() noexcept
{
	return this->Discriminate(m_Name, m_Slot);
}

bool engine::CTexture::HasAlpha() const noexcept
{
	return m_HasAlpha;
}
