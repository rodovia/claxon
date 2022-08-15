#include "BaseScene.h"
#include "SceneManager.h"
#include <tier0/Timer.h>
#include <engine_ui/Console.h>

engine::CConVar render_bg_r("render_bg_r", "0.07");
engine::CConVar render_bg_g("render_bg_g", "0.0");
engine::CConVar render_bg_b("render_bg_b", "0.12");

hl2::CBase_Scene::CBase_Scene(std::shared_ptr<CWindow> _Window)
	: m_Window(_Window)
{
	assert(m_Window != nullptr);
}

hl2::CBase_Scene::~CBase_Scene()
{}

float hl2::CBase_Scene::CreateFrame() noexcept
{
	float start = m_Timer.Mark().count();
	std::shared_ptr<engine::CGraphicalOutput> gr = this->GetWindow()->GetGraphicalOutput();

	gr->BeginFrame(render_bg_r.GetFloat(),
		render_bg_g.GetFloat(),
		render_bg_b.GetFloat());
	this->Render(start);
	engine::CConsole::SpawnWindow();
	gr->EndFrame();

	return m_Timer.Mark().count() - start;
}

std::shared_ptr<hl2::CWindow> hl2::CBase_Scene::GetWindow() const noexcept
{
	return m_Window;
}
