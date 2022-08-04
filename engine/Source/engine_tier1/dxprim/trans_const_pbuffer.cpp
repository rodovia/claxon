#include "TransformConstPixelBuffer.h"

engine::CTransformConstantPixelBuffer::CTransformConstantPixelBuffer(CGraphicalOutput& _Gfx,
																	 const CBase_Draw& _Parent,
																	 UINT _VS_Slot,
																	 UINT _PS_Slot) noexcept
	: CTransformConstantBuffer(_Gfx, _Parent, _PS_Slot)
{
	if (!s_PixelBuffer)
	{
		s_PixelBuffer = std::make_unique<CConstantPixelBuffer<Transforms>>(_Gfx, _PS_Slot);
	}
}

void engine::CTransformConstantPixelBuffer::Bind(CGraphicalOutput& _Gfx) noexcept
{
	const auto tf = this->GetTransforms(_Gfx);
	CTransformConstantBuffer::UpdateBindInner(_Gfx, tf);
	this->UpdateBindInner(_Gfx, tf);
}

void engine::CTransformConstantPixelBuffer::UpdateBindInner(CGraphicalOutput& _Gfx,
															const Transforms& _Tf) noexcept
{
	s_PixelBuffer->Update(_Gfx, _Tf);
	s_PixelBuffer->Bind(_Gfx);
}

std::unique_ptr<engine::CConstantPixelBuffer<engine::CTransformConstantBuffer::Transforms>> engine::CTransformConstantPixelBuffer::s_PixelBuffer;
