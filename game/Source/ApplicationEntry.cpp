#include "ApplicationEntry.h"
#include <InfoParser.h>
#include <imguihlp.h>

#include <engine_tier0/GDIPlusManager.h>

#include <scene/current/MainMenuScene.h>
#include <engine_tier1/VertexLayout.h>

#include <tier0/Timer.h>
#include <resource.h>
#include <random>

#define GETPATH(P) gamepaths::CMountedPaths::GetInstance().GetPath((P))
#define MAKEICON(I, R, X, Y) static_cast<HICON>(LoadImage((I), MAKEINTRESOURCE(R), IMAGE_ICON, (X), (Y), 0))

engine::CGDIPlusManager gdi;

hl2::CApplication::CApplication()
{
	m_Window = std::make_shared<CWindow>(800, 600, L"TAIKO NO TATSUJIN", engine::imgui::SetupImGui());
	CSceneManager::Instance().SetScene(std::make_shared<CScn_MainMenu>(m_Window));
}

hl2::CApplication::~CApplication()
{
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
	CSceneManager::Instance().ShowScene();
}

void hl2::CApplication::ShowRawMouseWindow()
{
	while (const auto d = m_Window->m_Mouse.ReadRawDelta())
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
