/* -*- c++ -*- */
#pragma once

#include <vector>
#include <engine_tier0/DLL.h>
#include <engine_tier1/GraphicalOutput.h>

#include <engine_tier1/VertexLayout.h>

namespace engine
{

class _ENGINE_DLLEXP CVertexBuffer : public CBase_Bind
{
public:
	template<class V>
	CVertexBuffer(CGraphicalOutput& _Gfx, const std::vector<V>& _Vertices)
		: m_Stride(UINT(sizeof(V)))
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(sizeof(V) * _Vertices.size());
		bd.StructureByteStride = sizeof(V);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = _Vertices.data();
		GetDevice(_Gfx)->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}

	CVertexBuffer(CGraphicalOutput& _Gfx, const layout::CVertexBuffer& _Buffer)
		: m_Stride(UINT(_Buffer.Layout().Size()))
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(_Buffer.Size());
		bd.StructureByteStride = m_Stride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = _Buffer.Data();
		GetDevice(_Gfx)->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}


	void Bind(CGraphicalOutput& _Gfx) override;
private:
	UINT m_Stride;
	CUtl_ComPtr<ID3D11Buffer> m_VertexBuffer;
};

}
