#pragma once

#include <memory>
#include <vector>

#include <engine_tier1/Camera.h>
#include <engine_tier1/LightPoint.h>
#include <engine_tier1/dxprim/draw/Box.h>
#include <engine_tier1/dxprim/draw/BaseDraw.h>

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
private:
	hl2::CWindow m_Window;
	hl2::CBasicTimer m_Timer;
	float m_SpeedFactor = 1.0f;
	engine::CCamera m_Cam;
	engine::CLightPoint m_Light;
	std::vector<std::unique_ptr<engine::CBase_Draw>> m_Drawables;
	static constexpr size_t s_Drawables = 30;
};

}
