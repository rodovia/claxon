#pragma once

#include "BaseDraw.h"

namespace engine
{

class CSolidSphere : public CBase_Draw
{
public:
	CSolidSphere(CGraphicalOutput& _Gfx, float _Radius);
	void SetPos(DirectX::XMFLOAT3 _Pos) noexcept;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
private:
	DirectX::XMFLOAT3 m_Pos = { 1.0f, 1.0f, 1.0f };
};

}
