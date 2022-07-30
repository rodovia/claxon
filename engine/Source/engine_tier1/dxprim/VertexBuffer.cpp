#include "VertexBuffer.h"
#include <typeinfo>

void engine::CVertexBuffer::Bind(CGraphicalOutput& _Gfx)
{
	const UINT off = 0u;
	GetContext(_Gfx)->IASetVertexBuffers(0u, 1u, m_VertexBuffer.GetAddressOf(), 
										&m_Stride, &off);
}

std::string engine::CVertexBuffer::Discriminate1(std::string _Tag)
{
	return typeid(engine::CVertexBuffer).name() + _Tag;
}

std::string engine::CVertexBuffer::GenerateDiscriminator() const noexcept
{
	return this->Discriminate1(m_Tag);
}