#include "TransformConstBuffer.h"
#include <typeinfo>

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
	this->UpdateBindInner(_Gfx, this->GetTransforms(_Gfx));
}

void engine::CTransformConstantBuffer::UpdateBindInner(CGraphicalOutput& _Gfx, const Transforms& _Tf) noexcept
{
	m_VertBuffer->Update(_Gfx, _Tf);
	m_VertBuffer->Bind(_Gfx);
}

engine::CTransformConstantBuffer::Transforms engine::CTransformConstantBuffer::GetTransforms(CGraphicalOutput& _Gfx) noexcept
{
	const auto modelView = m_Parent.GetTransformMatrix() * _Gfx.GetCamera();

	return {
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView * _Gfx.GetProjection())
	};
}

std::unique_ptr<engine::CConstantVertexBuffer<engine::CTransformConstantBuffer::Transforms>> engine::CTransformConstantBuffer::m_VertBuffer;
