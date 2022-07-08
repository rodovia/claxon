/* -*- c++ -*- */
#pragma once

#include <random>
#include <DirectXMath.h>
#include "BaseMiddlewareDraw.h"
#include <engine_tier0/DLL.h>

namespace engine
{

class _ENGINE_DLLEXP CBox : public CBase_MiddlewareDraw<CBox>
{
public:
	CBox(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
		std::uniform_real_distribution<float>& _Adist,
		std::uniform_real_distribution<float>& _Ddist,
		std::uniform_real_distribution<float>& _Odist,
		std::uniform_real_distribution<float>& _Rdist,
		std::uniform_real_distribution<float>& _Bdist,
		DirectX::XMFLOAT3 _Material);
	void Update(float _Dt) noexcept override;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
private:
	float m_R;

	struct 
	{
		float Roll = 0.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		float Theta;
		float Phi;
		float Chi;
	} m_Positional;

	struct
	{
		float dRoll;
		float dPitch;
		float dYaw;
		float dTheta;
		float dPhi;
		float dChi;
	} m_Speed;

	DirectX::XMFLOAT3X3 m_Matrix;
};

}

