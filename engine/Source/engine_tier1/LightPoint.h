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
	void Bind(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _View) noexcept;
private:
	struct CBLightPoint
	{
		alignas(16) DirectX::XMFLOAT3 Position;
		alignas(16) DirectX::XMFLOAT3 Ambient;
		alignas(16) DirectX::XMFLOAT3 DiffuseColor;
		float DiffuseIntensity, AttenuationConst, 
			AttenuationLinear, AttenuationQuadratic;
	};

	CBLightPoint m_ConstBufferData;
	mutable engine::CSolidSphere m_Mesh;
	mutable CConstantPixelBuffer<CBLightPoint> m_Buf;
};

}
