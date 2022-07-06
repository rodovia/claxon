/* -*- c++ -*- */
#pragma once

#include <engine_tier0/DLL.h>
#include "GraphicalOutput.h"

namespace engine
{

class _ENGINE_DLLEXP CCamera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset();
private:
	float m_R = 20.0f;

	struct
	{
		float Roll = 0.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		float Theta;
		float Phi;
	} m_Positional;
};

}
