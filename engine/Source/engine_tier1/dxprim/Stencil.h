// |===========================================|
// |Stencil.h - Depth stencil CBase_Bind abstr.|
// |===========================================|

#pragma once

#include <engine_tier0/BaseBind.h>
#include <engine_tier0/ComPtr.h>

namespace engine
{

enum class StencilMode
{
	Write,
	Read,
	Disabled
};

class CStencil : public CBase_Bind
{
public:
	CStencil(CGraphicalOutput& gfx, StencilMode md) noexcept
	{
		D3D11_DEPTH_STENCIL_DESC dc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
		switch (md)
		{
		case StencilMode::Write:
			dc.StencilEnable = TRUE;
			dc.StencilWriteMask = 0xFF;
			dc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			break;
		case StencilMode::Read:
			dc.DepthEnable = FALSE;
			dc.StencilEnable = TRUE;
			dc.StencilWriteMask = 0xFF;
			dc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
			dc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			break;
		default:
			break;
		}

		GetDevice(gfx)->CreateDepthStencilState(&dc, &m_State);
	}

	void Bind(CGraphicalOutput& gfx) override
	{
		GetContext(gfx)->OMSetDepthStencilState(m_State.Get(), 0xFF);
	}

private:
	CUtl_ComPtr<ID3D11DepthStencilState> m_State;
};

}
