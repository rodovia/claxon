#pragma once

#include <engine_tier1/dxprim/draw/BaseMiddlewareDraw.h>
#include <random>

namespace engine
{

template<class T>
class CBase_PrimObject : public CBase_MiddlewareDraw<T>
{
public:
	CBase_PrimObject() = default;
	CBase_PrimObject(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
			std::uniform_real_distribution<float>& _Adist,
			std::uniform_real_distribution<float>& _Ddist,
			std::uniform_real_distribution<float>& _Odist,
			std::uniform_real_distribution<float>& _Rdist,
			std::uniform_real_distribution<float>& _Bdist
	)
		: m_Speed({
			_Ddist(_Rng), /* dRoll */
			_Ddist(_Rng), /* dPitch */
			_Ddist(_Rng), /* dYaw */
			_Odist(_Rng), /* dTheta */
			_Odist(_Rng), /* dPhi */
			_Odist(_Rng), /* dChi */
			}),
			m_Positional({
				  0.0f,		  /* Roll */
				  0.0f,		  /* Pitch */
				  0.0f,		  /* Yaw */
				  _Adist(_Rng), /* Theta */
				  _Adist(_Rng), /* Phi */
				  _Adist(_Rng), /* Chi */
				}),
				m_R(_Rdist(_Rng))
	{};

	void Update(float _Dt) noexcept
	{
		m_Positional.Roll += m_Speed.dRoll * _Dt;
		m_Positional.Pitch += m_Speed.dPitch * _Dt;
		m_Positional.Yaw += m_Speed.dYaw * _Dt;
		m_Positional.Theta += m_Speed.dTheta * _Dt;
		m_Positional.Phi += m_Speed.dPhi * _Dt;
		m_Positional.Chi += m_Speed.dChi * _Dt;
	};

	DirectX::XMMATRIX GetTransformMatrix() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Pitch, m_Positional.Yaw, m_Positional.Roll) *
			DirectX::XMMatrixTranslation(m_R, 0.0f, 0.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Theta, m_Positional.Phi, m_Positional.Chi);
	}
protected:
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
