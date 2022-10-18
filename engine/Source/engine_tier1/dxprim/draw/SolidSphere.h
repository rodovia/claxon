#pragma once

#include "BaseDraw.h"
#include <engine_tier1/Camera.h>
#include <engine_tier0/DLL.h>

namespace engine
{

class _ENGINE_DLLEXP CSolidSphere : public CBase_Draw
{
public:
	CSolidSphere(CGraphicalOutput& _Gfx, float _Radius);
	void SetPos(DirectX::XMFLOAT3 _Pos) noexcept;
	DirectX::XMMATRIX GetTransformMatrix(const CCamera&) const noexcept override;
private:
	DirectX::XMFLOAT3 m_Pos = { 1.0f, 1.0f, 1.0f };
};

}
