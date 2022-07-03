#pragma once

#include <engine_tier0/DLL.h>
#include "BaseMiddlewareDraw.h"

namespace engine
{

class CSolidSphere : public CBase_MiddlewareDraw<CSolidSphere>
{
public:
	CSolidSphere(CGraphicalOutput& _Gfx, float _Radius);
	void Update(float _Dt) noexcept override;
	void SetPos(DirectX::XMFLOAT3 _Pos) noexcept;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
private:
	DirectX::XMFLOAT3 m_Pos = { 1.0f, 1.0f, 1.0f };
};

}

