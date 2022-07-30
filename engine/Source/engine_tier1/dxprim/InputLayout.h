/* -*- c++ -*- */
#pragma once

#include <vector>
#include <engine_tier0/DLL.h>
#include <engine_tier1/VertexLayout.h>
#include <engine_tier1/GraphicalOutput.h>

#define _ENGINE_POSITION_IED { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }

namespace engine
{

class CInputLayout : public CBase_Bind
{
public:
	CInputLayout(engine::CGraphicalOutput& _Gfx,
				engine::layout::CVertexLayout _Layout,
				ID3DBlob* _BlobBytecode);
	void Bind(CGraphicalOutput&) override;
	static std::string Discriminate(layout::CVertexLayout _Layout, ID3DBlob* _Ignored);
	std::string GenerateDiscriminator() const noexcept override;
private:
	engine::layout::CVertexLayout m_Layout;
	CUtl_ComPtr<ID3D11InputLayout> m_InLayout;
};

}
