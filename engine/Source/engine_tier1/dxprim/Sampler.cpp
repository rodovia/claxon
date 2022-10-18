#include "Sampler.h"
#include <typeinfo>
#include <engine_ui/Console.h>
#include <engine_tier0/Exceptions.h>

engine::CConVar samp_anit_filter("samp_anit_filter", "0");

engine::CSampler::CSampler(CGraphicalOutput& _Gfx)
{
	bool hasAnitFilter = samp_anit_filter.GetInteger() > 0;
	if (samp_anit_filter.GetInteger() > 16)
	{
		Log("samp_anit_filter > 16! Disabling.");
		hasAnitFilter = false;
	}

	D3D11_SAMPLER_DESC sde = {};
	sde.Filter = hasAnitFilter ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sde.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sde.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sde.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (hasAnitFilter)
	{
		sde.MaxAnisotropy = (int64_t) samp_anit_filter;
		sde.MipLODBias = 0.0f;
	}
	sde.MinLOD = 0.0;
	sde.MaxLOD = FLT_MAX;
	HRESULT hr;
	_ENGINE_MAYTHROW_GRAPHICS(GetDevice(_Gfx)->CreateSamplerState(&sde, &m_Sampler));
}

std::string engine::CSampler::Discriminate() noexcept
{
	return typeid(CSampler).name();
}

std::string engine::CSampler::GenerateDiscriminator() const noexcept
{
	return this->Discriminate();
}

void engine::CSampler::Bind(CGraphicalOutput& _Gfx) noexcept
{
	GetContext(_Gfx)->PSSetSamplers(0u, 1u, m_Sampler.GetAddressOf());
}
