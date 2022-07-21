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
	: m_Window(1024, 768, L"TAIKO NO TATSUJIN", engine::imgui::SetupImGui()),
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
	const auto dt = m_Timer.Mark().count();

	if (m_Window.m_Keyboard.IsKeyPressed(VK_F1))
	{
		m_Window.ToggleCursorDisplay();
		m_ShowDemoWindow = !m_ShowDemoWindow;
	}

	if (!m_Window.CursorEnabled())
	{
		if (m_Window.m_Keyboard.IsKeyPressed('W'))
		{
			m_Cam.Translate({ 0.0f, 0.0f, dt });
		}
		if (m_Window.m_Keyboard.IsKeyPressed('A'))
		{
			m_Cam.Translate({ -dt, 0.0f, 0.0f });
		}
		if (m_Window.m_Keyboard.IsKeyPressed('S'))
		{
			m_Cam.Translate({ 0.0f, 0.0f, -dt });
		}
		if (m_Window.m_Keyboard.IsKeyPressed('D'))
		{
			m_Cam.Translate({ dt, 0.0f, 0.0f });
		}
		if (m_Window.m_Keyboard.IsKeyPressed('R'))
		{
			m_Cam.Translate({ 0.0f, dt, 0.0f});
		}
		if (m_Window.m_Keyboard.IsKeyPressed('F'))
		{
			m_Cam.Translate({ 0.0f, -dt, 0.0f });
		}

		while (const auto delta = m_Window.m_Mouse.ReadRawDelta())
		{
			m_Cam.Rotate(delta->X, delta->Y);
		}
	}

	m_Window.Graphics().BeginFrameNorm(200, 200, 200);
	m_Window.Graphics().SetCamera(m_Cam.GetMatrix());
	m_Light.Bind(m_Window.Graphics(), m_Cam.GetMatrix());

	m_Nano.Draw(m_Window.Graphics());
	m_Light.Draw(m_Window.Graphics());

	if (m_ShowDemoWindow)
	{
		m_Light.SpawnControlWindow();
		m_Cam.SpawnControlWindow();
		m_Nano.ShowDiagWindow("nanosuit.obj");
		engine::CConsole::SpawnWindow();
	}

	m_Window.Graphics().EndFrame();
}

void hl2::CApplication::ShowRawMouseWindow()
{
	while (const auto d = m_Window.m_Mouse.ReadRawDelta())
	{
		m_X += d->X;
		m_Y += d->Y;
	}

	if (ImGui::Begin("Entrada nao-processada do rato"))
	{
		ImGui::Text("Talimo: (%d, %d)", m_X, m_Y);
	}
	ImGui::End();
}
