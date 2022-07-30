/* -*- c++ -*- */
#pragma once

#include <engine_tier0/BaseBind.h>
#include <engine_tier0/ComPtr.h>

namespace engine
{

class CSampler : public CBase_Bind
{
public:
	CSampler(CGraphicalOutput& _Gfx);
	void Bind(CGraphicalOutput& _Gfx) noexcept override;
	static std::string Discriminate() noexcept;
	std::string GenerateDiscriminator() const noexcept;
protected:
	CUtl_ComPtr<ID3D11SamplerState> m_Sampler;
};

}
