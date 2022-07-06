/* -*- c++ -*- */
#pragma once

#include <random>
#include <engine_tier0/DLL.h>
#include "BaseMiddlewareDraw.h"

namespace engine
{

class _ENGINE_DLLEXP CMelon : public CBase_MiddlewareDraw<CMelon>
{
public:
	CMelon(CGraphicalOutput& _Gfx,
		std::mt19937& _Rng,
		std::uniform_real_distribution<float>& _Adist,
		std::uniform_real_distribution<float>& _Ddist,
		std::uniform_real_distribution<float>& _Odist,
		std::uniform_real_distribution<float>& _Rdist,
		std::uniform_int_distribution<int>& _LonDist,
		std::uniform_int_distribution<int>& _LatDist);
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
};

}
