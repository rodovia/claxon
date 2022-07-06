#include "ApplicationEntry.h"

#include <InfoParser.h>

#include <imguihlp.h>
#include <engine_tier0/_Math.h>

#include <engine_tier0/GDIPlusManager.h>
#include <engine_tier1/Surface.h>
#include <engine_tier1/dxprim/draw/SkinnedBox.h>
#include <engine_tier1/dxprim/draw/Pyramid.h>
#include <engine_tier1/dxprim/draw/Box.h>
#include <engine_tier1/dxprim/draw/Melon.h>
#include <engine_tier1/dxprim/draw/Sheet.h>

#include <tier0/Timer.h>
#include <resource.h>
#include <random>

#define GETPATH(P) gamepaths::CMountedPaths::GetInstance().GetPath((P))
#define MAKEICON(I, R, X, Y) static_cast<HICON>(LoadImage((I), MAKEINTRESOURCE(R), IMAGE_ICON, (X), (Y), 0))

engine::CGDIPlusManager gdi;

hl2::CApplication::CApplication() 
	: m_Window(800, 600, L"TAIKO NO TATSUJIN", engine::imgui::SetupImGui()),
	  m_Light(m_Window.Graphics())
{
	class Factory
	{
	public:
		Factory(engine::CGraphicalOutput& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<engine::CBase_Draw> operator()()
		{
			return std::make_unique<engine::CBox>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			
		}
	private:
		engine::CGraphicalOutput& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI_f * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI_f * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI_f * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,2 };
	};

	Factory f(m_Window.Graphics());
	m_Drawables.reserve(s_Drawables);
	std::generate_n(std::back_inserter(m_Drawables),s_Drawables, f);

	gamepaths::CInfoParser::FromFile("gameinfo.txt");
	m_Window.Graphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
	
}

#pragma warning(disable : 4244) // conversion from 'WPARAM' to 'int' - possible data loss.

int hl2::CApplication::Main()
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

#pragma warning(pop)

void hl2::CApplication::FrameLoop()
{
	auto dt = m_Timer.Mark() * m_SpeedFactor;
	m_Window.Graphics().BeginFrameNorm(255, 127, 0);
	m_Window.Graphics().SetCamera(m_Cam.GetMatrix());
	m_Light.Bind(m_Window.Graphics());

	for (auto& b : m_Drawables)
	{
		b->Update(m_Window.m_Keyboard.IsKeyPressed(VK_SPACE) ? 0.0f : dt.count());
		b->Draw(m_Window.Graphics());
	}
	m_Light.Draw(m_Window.Graphics());

	if (ImGui::Begin("Simulation Options"))
	{
		ImGui::SliderFloat("Speed factor", &m_SpeedFactor, 0.0f, 4.0f);
		ImGui::Text("Application avg: %.3f ms/frame (%.1f FPS)", ImGui::GetIO().Framerate / 1000, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	m_Cam.SpawnControlWindow();
	m_Light.SpawnControlWindow();

	m_Window.Graphics().EndFrame();
}
