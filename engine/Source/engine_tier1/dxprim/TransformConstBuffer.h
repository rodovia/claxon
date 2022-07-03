#pragma once

#include "ConstantBuffer.h"
#include <engine_tier0/DLL.h>
#include "draw/BaseDraw.h"
#include <memory>
#include <DirectXMath.h>

namespace engine
{

class _ENGINE_DLLEXP CTransformConstantBuffer : public CBase_Bind
{
public:
	CTransformConstantBuffer(CGraphicalOutput& _Gfx, const CBase_Draw& _Parent);
	void Bind(CGraphicalOutput&) override;
private:
	static std::unique_ptr<CConstantVertexBuffer<DirectX::XMMATRIX>> m_VertBuffer;
	const CBase_Draw& m_Parent;
};

}
