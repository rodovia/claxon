#pragma once

#include "TransformConstBuffer.h"

namespace engine
{

// TODO: Find a better name for this class...
class CTransformConstantPixelBuffer : public CTransformConstantBuffer
{
public:
	CTransformConstantPixelBuffer(CGraphicalOutput& _Gfx, const CBase_Draw& _Parent, UINT _VS_Slot = 0u,
								  UINT _PS_Slot = 0u) noexcept;
	void Bind(CGraphicalOutput& _Gfx) noexcept override;

protected:
	void UpdateBindInner(CGraphicalOutput& _Gfx, const Transforms& _Tf) noexcept;

private:
	static std::unique_ptr<CConstantPixelBuffer<Transforms>> s_PixelBuffer;
};

} // namespace engine
