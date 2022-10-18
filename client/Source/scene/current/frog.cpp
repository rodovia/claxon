#pragma once

#include "FrogScene.h"
#include <engine_ui/Console.h>
#include <tier1/Window.hh>

extern engine::CConVar r_drawdist;


void hl2::CFrogScene::Start()
{
	auto ptr = this->GetWindow();
	auto ptr2 = ptr->GetGraphicalOutput();
	
	ptr2->SetProjection(
		DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, r_drawdist)
	);
	engine::SPRITE_DESCRIPTOR dc;
	dc.InitialPos = { 0.0f, 0.0f, 0.0f };
	m_Spr.emplace(*ptr2, _GETPATH("resources/textures/jumpin.png"), dc);
	m_Mdl.emplace(*ptr2, _GETPATH("resources/model/city/city_triangulated.obj"));
	m_Sph.emplace(*ptr2.get(), 1.0f, DirectX::XMFLOAT4(0.3f, 0.3f, 1.0f, 1.0f));
	m_Sph->SetPos(DirectX::XMFLOAT3(0,0,0));
}

void hl2::CFrogScene::Render(float _Dt)
{
	static bool getposwo = true;
	float dt = m_Timer.Mark().count();
	std::shared_ptr<CWindow> wnd = this->GetWindow();
	wnd->GetGraphicalOutput()->SetCamera(m_Cam);

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

	if (wnd->m_Keyboard.IsKeyPressed('Z'))
	{
		wnd->ToggleCursorDisplay();
		wnd->m_Mouse.ToggleRawCapture();
	}

	m_Mdl->Draw(*wnd->GetGraphicalOutput());
	m_Sph->Draw(*wnd->GetGraphicalOutput());
	m_Sph->SpawnTestWindow(*wnd->GetGraphicalOutput());
	m_Cam.SpawnControlWindow();
}
