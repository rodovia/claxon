#include "TransformConstBuffer.h"

engine::CTransformConstantBuffer::CTransformConstantBuffer(
			CGraphicalOutput& _Gfx, const CBase_Draw& _Parent, UINT _Slot)
	: m_Parent(_Parent)
{
	if (!m_VertBuffer)
	{
		m_VertBuffer = std::make_unique<CConstantVertexBuffer<CTransformConstantBuffer::Transforms>>(_Gfx, _Slot);
	}
}

void engine::CTransformConstantBuffer::Bind(CGraphicalOutput& _Gfx)
{
	const auto model = m_Parent.GetTransformMatrix();
	Transforms tf{
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(
			model *
			_Gfx.GetCamera() *
			_Gfx.GetProjection()
		)
	};
	
	m_VertBuffer->Update(_Gfx, tf);
	m_VertBuffer->Bind(_Gfx);
}

std::unique_ptr<engine::CConstantVertexBuffer<engine::CTransformConstantBuffer::Transforms>> engine::CTransformConstantBuffer::m_VertBuffer;
