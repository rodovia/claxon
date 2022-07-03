#include "Sampler.h"

engine::CSampler::CSampler(CGraphicalOutput& _Gfx)
{
	D3D11_SAMPLER_DESC sde = {};
	sde.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sde.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sde.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sde.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GetDevice(_Gfx)->CreateSamplerState(&sde, &m_Sampler);
}

void engine::CSampler::Bind(CGraphicalOutput& _Gfx) noexcept
{
	GetContext(_Gfx)->PSSetSamplers(0u, 1u, m_Sampler.GetAddressOf());
}