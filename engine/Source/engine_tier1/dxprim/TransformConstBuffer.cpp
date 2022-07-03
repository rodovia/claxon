#include "TransformConstBuffer.h"

engine::CTransformConstantBuffer::CTransformConstantBuffer(
			CGraphicalOutput& _Gfx, const CBase_Draw& _Parent)
	: m_Parent(_Parent)
{
	if (!m_VertBuffer)
	{
		m_VertBuffer = std::make_unique<CConstantVertexBuffer<DirectX::XMMATRIX>>(_Gfx);
	}
}

void engine::CTransformConstantBuffer::Bind(CGraphicalOutput& _Gfx)
{
	m_VertBuffer->Update(_Gfx, DirectX::XMMatrixTranspose(
		m_Parent.GetTransformMatrix() *
		_Gfx.GetCamera() * 
		_Gfx.GetProjection()
	));
	m_VertBuffer->Bind(_Gfx);
}

std::unique_ptr<engine::CConstantVertexBuffer<DirectX::XMMATRIX>> engine::CTransformConstantBuffer::m_VertBuffer;
