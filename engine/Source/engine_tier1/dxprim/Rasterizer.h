#pragma once

#include <engine_tier0/BaseBind.h>
#include <engine_tier0/ComPtr.h>
#include <string>

namespace engine
{

class CRasterizer : public CBase_Bind
{
public:
	CRasterizer(CGraphicalOutput& _Gfx, bool _RenderBack);
	void Bind(CGraphicalOutput& _Gfx) override;
	std::string GenerateDiscriminator() noexcept override;
	static std::string Discriminate(bool) noexcept;
private:
	bool m_RenderBack;
	CUtl_ComPtr<ID3D11RasterizerState> m_Rstate;
};

}
