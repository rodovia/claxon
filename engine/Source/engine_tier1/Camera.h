/* -*- c++ -*- */
#pragma once

#include <engine_tier0/DLL.h>
#include "GraphicalOutput.h"

namespace engine
{

class _ENGINE_DLLEXP CCamera
{
public:
	CCamera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset();
	void Rotate(float _DeltaX, float _DeltaY) noexcept;
	void Translate(DirectX::XMFLOAT3 _Position) noexcept;
private:
	DirectX::XMFLOAT3 m_Position;
	float m_Pitch, m_Yaw;

	static constexpr float s_TravelSpeed = 12.0f;
	static constexpr float s_RotationSpeed = 0.004f;
};

}
