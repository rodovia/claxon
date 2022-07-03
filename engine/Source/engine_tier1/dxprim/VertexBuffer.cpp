#include "VertexBuffer.h"

void engine::CVertexBuffer::Bind(CGraphicalOutput& _Gfx)
{
	const UINT off = 0u;
	GetContext(_Gfx)->IASetVertexBuffers(0u, 1u, m_VertexBuffer.GetAddressOf(), 
										&m_Stride, &off);
}

