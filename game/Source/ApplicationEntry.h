#pragma once

#include <memory>
#include <vector>

#include <engine_tier1/Surface.h>
#include <engine_tier1/Camera.h>
#include <engine_tier1/LightPoint.h>
#include <engine_tier1/dxprim/draw/Box.h>
#include <engine_tier1/dxprim/draw/BaseDraw.h>
#include <engine_tier1/Model.h>

#include "tier1/Window.hh"
#include <tier0/Timer.h>

namespace hl2
{

class CApplication
{
public:
	CApplication();
	WPARAM Main();
private:
	void FrameLoop();
	void SpawnModelWindow();
private:
	hl2::CWindow m_Window;
	engine::CCamera m_Cam;
	engine::CModel m_Nano { m_Window.Graphics(), _GETPATH("resources/model/nanosuit.obj") };
	engine::CLightPoint m_Light;

	struct
	{
		float Roll = 0.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		float X = 0.0f;
		float Y = 0.0f;
		float Z = 0.0f;
	} m_Pos;
};

}
