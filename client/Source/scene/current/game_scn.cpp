#include "GameScene.h"
#include <engine_ui/Console.h>
#include <engine_tier0/_Math.h>

engine::CConVar mdl_name("mdl_name", "resources/model/sponza/sponza.obj");

engine::CConVar r_fov("r_fov", "180");
engine::CConVar r_drawdist("r_drawdist", "40.0");

void hl2::CScene_Game::Start()
{
	ResolutionWH wh = this->GetWindow()->GetSize();
	engine::MODEL_DESCRIPTOR mdldesc = {0};
	mdldesc.Scale = 1.0f;
	m_Wall.emplace(*GetWindow()->GetGraphicalOutput(), _GetPath(mdl_name.GetString()), mdldesc);

	GetWindow()->GetGraphicalOutput()->SetProjection(
		DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, r_drawdist)
	);
}

void hl2::CScene_Game::End()
{
	m_Wall = std::nullopt;
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

	GetWindow()->GetGraphicalOutput()->SetCamera(m_Cam);
	m_Light.Bind(*GetWindow()->GetGraphicalOutput(), m_Cam.GetMatrix());

	m_Wall->Draw(*wnd->GetGraphicalOutput());
	m_Light.Draw(*wnd->GetGraphicalOutput());

	if (m_ShowDemoWindow)
	{
		m_Light.SpawnControlWindow();
		m_Cam.SpawnControlWindow();
		m_Wall->ShowDiagWindow(*wnd->GetGraphicalOutput(), "brick_wall.obj");
	}
}
