#pragma once

#include <engine_tier0/BaseBind.h>
#include <wrl.h>

namespace engine
{

class _ENGINE_DLLEXP CSampler : public CBase_Bind
{
public:
	CSampler(CGraphicalOutput& _Gfx);
	void Bind(CGraphicalOutput& _Gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_Sampler;
};

}
