/* -*- c++ -*- */
#pragma once

#include <random>
#include <DirectXMath.h>
#include <engine_tier0/BasePrimitiveObject.h>
#include <engine_tier0/DLL.h>

namespace engine
{

class _ENGINE_DLLEXP CBox : public CBase_PrimObject<CBox>
{
public:
	CBox(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
		std::uniform_real_distribution<float>& _Adist,
		std::uniform_real_distribution<float>& _Ddist,
		std::uniform_real_distribution<float>& _Odist,
		std::uniform_real_distribution<float>& _Rdist,
		std::uniform_real_distribution<float>& _Bdist,
		DirectX::XMFLOAT3 _Material);
	DirectX::XMMATRIX GetTransformMatrix() const noexcept;
private:
	DirectX::XMFLOAT3X3 m_Matrix;
};

}

