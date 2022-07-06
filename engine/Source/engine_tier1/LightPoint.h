/* -*- c++ -*- */
#pragma once

#include <engine_tier0/DLL.h>
#include <engine_tier1/dxprim/draw/SolidSphere.h>
#include "dxprim/ConstantBuffer.h"
#include "GraphicalOutput.h"

namespace engine
{

class _ENGINE_DLLEXP CLightPoint
{
public:
	CLightPoint(CGraphicalOutput& _Gfx, float _Radius = 0.5f);
	void SpawnControlWindow();
	void Reset() noexcept;
	void Draw(CGraphicalOutput& _Gfx) noexcept;
	void Bind(CGraphicalOutput& _Gfx) const noexcept;
private:
	struct CBLightPoint
	{
		DirectX::XMFLOAT3 pos;
		float padding;
	};

	DirectX::XMFLOAT3 m_Pos = { 0.0f, 0.0f, 0.0f };
	engine::CSolidSphere m_Mesh;
	mutable CConstantPixelBuffer<CBLightPoint> m_Buf;
};

}
