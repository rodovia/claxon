#pragma once

#include <vector>
#include <engine_tier0/DLL.h>
#include <engine_tier1/GraphicalOutput.h>

#define _ENGINE_POSITION_IED { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }

namespace engine
{

class _ENGINE_DLLEXP CInputLayout : public CBase_Bind
{
public:
	CInputLayout(engine::CGraphicalOutput& _Gfx,
				const std::vector<D3D11_INPUT_ELEMENT_DESC>& _Layout,
				ID3DBlob* _BlobBytecode);
	void Bind(CGraphicalOutput&) override;
private:
	ComPtr<ID3D11InputLayout> m_InLayout;
};

}
