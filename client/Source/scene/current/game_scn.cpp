#include "GameScene.h"
#include <engine_ui/Console.h>

void hl2::CScene_Game::Start()
{
	GetWindow()->Graphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

void hl2::CScene_Game::Render(float _Dt)
{
	std::shared_ptr<CWindow> wnd = this->GetWindow();
	const auto dt = m_Timer.Mark().count();

	if (wnd->m_Keyboard.IsKeyPressed(VK_F1))
	{
		wnd->ToggleCursorDisplay();
		wnd->m_Mouse.ToggleRawCapture();
		// m_ShowDemoWindow = !m_ShowDemoWindow;
	}

	if (!wnd->CursorEnabled())
	{
		if (wnd->m_Keyboard.IsKeyPressed('W'))
		{
			m_Cam.Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd->m_Keyboard.IsKeyPressed('A'))
		{
			m_Cam.Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd->m_Keyboard.IsKeyPressed('S'))
		{
			m_Cam.Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd->m_Keyboard.IsKeyPressed('D'))
		{
			m_Cam.Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd->m_Keyboard.IsKeyPressed('R'))
		{
			m_Cam.Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd->m_Keyboard.IsKeyPressed('F'))
		{
			m_Cam.Translate({ 0.0f, -dt, 0.0f });
		}

		while (const auto delta = GetWindow()->m_Mouse.ReadRawDelta())
		{
			m_Cam.Rotate(delta->X, delta->Y);
		}
	}


	GetWindow()->Graphics().SetCamera(m_Cam.GetMatrix());
	m_Light.Bind(GetWindow()->Graphics(), m_Cam.GetMatrix());

	m_Wall.Draw(wnd->Graphics());
	m_Light.Draw(wnd->Graphics());

	if (m_ShowDemoWindow)
	{
		m_Light.SpawnControlWindow();
		m_Cam.SpawnControlWindow();
		m_Wall.ShowDiagWindow("brick_wall.obj");
		engine::CConsole::SpawnWindow();
	}
}
