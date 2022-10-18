#pragma once

#include <engine_tier0/DLL.h>
#include "BaseDraw.h"

namespace engine
{

class _ENGINE_DLLEXP CPlane : public CBase_Draw
{
public:
	CPlane(CGraphicalOutput& _Gfx, float _Size, DirectX::XMFLOAT4 _Color = {1.0f, 1.0f, 1.0f, 1.0f});
	void SetPos(DirectX::XMFLOAT3 _Pos) noexcept;
	void SetRotation(DirectX::XMFLOAT3 _NewRot) noexcept;
	DirectX::XMMATRIX GetTransformMatrix(const CCamera&) const noexcept override;
	void SpawnTestWindow(CGraphicalOutput& _Gfx) noexcept;
private:
	DirectX::XMFLOAT3 m_Pos, m_Rotation = { 0, 0, 0 };
	bool m_IsBlending = true;
	struct __declspec(align(16)) PSConstantBuffer
	{
		DirectX::XMFLOAT4 Color;
	} m_Pmc;
};

}
