#pragma once

#include <engine_tier0/DLL.h>
#include "BaseDraw.h"

namespace engine
{

class _ENGINE_DLLEXP CPlane : public CBase_Draw
{
public:
	CPlane(CGraphicalOutput& _Gfx, float _Size);
	void SetPos(DirectX::XMFLOAT3 _Pos) noexcept;
	void SetRotation(DirectX::XMFLOAT3 _NewRot) noexcept;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
	void SpawnTestWindow(CGraphicalOutput& _Gfx) noexcept;
private:
	DirectX::XMFLOAT3 m_Pos, m_Rotation;

	struct __declspec(align(16)) PSConstantBuffer
	{
		float SpecularIntensity = 20.0f;
		float SpecularPower = 0.6f;
		BOOL EnableNormalMap = TRUE;
		// float _padding[1];
	} m_Pmc;
};

}
