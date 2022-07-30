/* -*- c++ -*- */
#pragma once

#include <vector>
#include <engine_tier0/DLL.h>
#include <engine_tier1/GraphicalOutput.h>
#include <engine_tier0/Exceptions.h>

#include <engine_tier1/VertexLayout.h>

namespace engine
{

class CVertexBuffer : public CBase_Bind
{
public:
	CVertexBuffer(CGraphicalOutput& _Gfx, std::string _Tag, const layout::CVertexBuffer& _Buffer)
		: m_Stride(UINT(_Buffer.Layout().Size())),
		m_Tag(_Tag)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(_Buffer.SizeBytes());
		bd.StructureByteStride = m_Stride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = _Buffer.Data();
		HRESULT hr;
		_ENGINE_MAYTHROW_GRAPHICS(GetDevice(_Gfx)->CreateBuffer(&bd, &sd, &m_VertexBuffer));
	}

	void Bind(CGraphicalOutput& _Gfx) override;
	std::string GenerateDiscriminator() const noexcept;

	template<class..._Ty_Ignore>
	static std::string Discriminate(std::string _Tag, _Ty_Ignore&&...)
	{
		return Discriminate1(_Tag);
	}
private:
	static std::string Discriminate1(std::string _Tag);
private:
	std::string m_Tag;
	UINT m_Stride;
	CUtl_ComPtr<ID3D11Buffer> m_VertexBuffer;
};

}
