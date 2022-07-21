#include "ApplicationEntry.h"

#include <InfoParser.h>

#include <imguihlp.h>
#include <engine_tier0/_Math.h>

#include <engine_tier0/GDIPlusManager.h>
#include <engine_tier1/Surface.h>
#include <engine_ui/Console.h>

#include <engine_tier1/VertexLayout.h>

#include <tier0/Timer.h>
#include <resource.h>
#include <random>

#define GETPATH(P) gamepaths::CMountedPaths::GetInstance().GetPath((P))
#define MAKEICON(I, R, X, Y) static_cast<HICON>(LoadImage((I), MAKEINTRESOURCE(R), IMAGE_ICON, (X), (Y), 0))

engine::CGDIPlusManager gdi;

engine::CConCmd debug_break("debug_break", []
(std::vector<std::string> _Ignored)
	{
		engine::CConsole::Instance().EmitMessage("Check your debugger...");
		__debugbreak();
	});

hl2::CApplication::CApplication() 
	: m_Window(800, 600, L"TAIKO NO TATSUJIN", engine::imgui::SetupImGui()),
	  m_Light(m_Window.Graphics())
{
	gamepaths::CInfoParser::FromFile("gameinfo.txt");
	m_Window.Graphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

WPARAM hl2::CApplication::Main()
{
	while (true)
	{
		if (const auto ecode = hl2::CWindow::ProcessMessage())
		{
			return *ecode;
		}
		this->FrameLoop();
	}
}

void hl2::CApplication::FrameLoop()
{
	m_Window.Graphics().BeginFrameNorm(200, 200, 200);
	m_Window.Graphics().SetCamera(m_Cam.GetMatrix());
	m_Light.Bind(m_Window.Graphics(), m_Cam.GetMatrix());

	m_Nano.Draw(m_Window.Graphics());
	m_Light.Draw(m_Window.Graphics());

	m_Light.SpawnControlWindow();
	m_Cam.SpawnControlWindow();
	m_Nano.ShowDiagWindow("nanosuit.obj");
	engine::CConsole::SpawnWindow();

	m_Window.Graphics().EndFrame();
}

