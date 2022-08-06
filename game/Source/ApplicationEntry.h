#pragma once

#include <memory>
#include <vector>

#include <scene/SceneManager.h>

#include "tier1/Window.hh"
#include <tier0/Timer.h>

namespace hl2
{

class CApplication
{
public:
	CApplication();
	~CApplication();
	WPARAM Main();
private:
	void FrameLoop();
	void ShowRawMouseWindow();
private:
	CSceneManager* m_ScnMgr;
	std::shared_ptr<CWindow> m_Window;
	int m_X = 0, m_Y = 0;
};

}
