#pragma once

#include <memory>
#include <vector>

#include <engine_tier1/Surface.h>
#include <engine_tier1/Camera.h>
#include <engine_tier1/LightPoint.h>
#include <engine_tier1/dxprim/draw/BaseDraw.h>
#include <engine_tier1/Model.h>
#include <engine_tier1/dxprim/draw/TestPlane.h>

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
	void ShowRawMouseWindow();
private:
	hl2::CWindow m_Window;
	engine::CCamera m_Cam;
	engine::CModel m_Wall { m_Window.Graphics(), _GETPATH("resources/model/urban_misc/brick_wall.obj") };
	engine::CLightPoint m_Light;
	hl2::CBasicTimer m_Timer;
	bool m_ShowDemoWindow = true;

	int m_X = 0, m_Y = 0;
};

}
