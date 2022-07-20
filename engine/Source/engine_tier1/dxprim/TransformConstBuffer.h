/* -*- c++ -*- */
#pragma once

#include "ConstantBuffer.h"
#include <engine_tier0/DLL.h>
#include "draw/BaseDraw.h"
#include <memory>
#include <DirectXMath.h>

namespace engine
{

class CTransformConstantBuffer : public CBase_Bind
{
public:
	CTransformConstantBuffer(CGraphicalOutput& _Gfx, const CBase_Draw& _Parent, UINT _Slot = 0);
	void Bind(CGraphicalOutput&) override;
private:
	struct Transforms
	{
		DirectX::XMMATRIX ModelViewProj;
		DirectX::XMMATRIX Model;
	};

	static std::unique_ptr<CConstantVertexBuffer<Transforms>> m_VertBuffer;
	const CBase_Draw& m_Parent;
};

}

