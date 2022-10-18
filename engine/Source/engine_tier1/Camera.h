/* -*- c++ -*- */
#pragma once

#include <engine_tier0/DLL.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace engine
{

struct _Pair
{
	float x, y;
};

class _ENGINE_DLLEXP CCamera
{
public:
	CCamera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset();
	void Rotate(float _DeltaX, float _DeltaY) noexcept;
	void Translate(DirectX::XMFLOAT3 _Position) noexcept;
	_Pair GetPitchYaw() const noexcept;
	DirectX::XMFLOAT3 GetPosition() const noexcept;

private:
	DirectX::XMFLOAT3 m_Position;
	float m_Pitch, m_Yaw;

	static constexpr float s_TravelSpeed = 12.0f;
	static constexpr float s_RotationSpeed = 0.004f;
};

}
