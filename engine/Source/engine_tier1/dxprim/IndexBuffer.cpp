#include "IndexBuffer.h"

engine::CIndexBuffer::CIndexBuffer(engine::CGraphicalOutput& _Gfx, 
									std::vector<unsigned short> _Indices)
	: m_IndicesCount((UINT) _Indices.size())
{
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(m_IndicesCount * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(USHORT);

	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = _Indices.data();
	GetDevice(_Gfx)->CreateBuffer(&ibd, &isd, &m_IndexBuffer);
}

UINT engine::CIndexBuffer::GetCount() const noexcept
{
	return m_IndicesCount;
}

void engine::CIndexBuffer::Bind(engine::CGraphicalOutput& _Gfx)
{
	GetContext(_Gfx)->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}
