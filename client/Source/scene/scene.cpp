#include "BaseScene.h"
#include "SceneManager.h"
#include <tier0/Timer.h>

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

	m_Window->Graphics().BeginFrame(1.0f, 1.0f, 0.0f);
	this->Render(start);
	m_Window->Graphics().EndFrame();

	return m_Timer.Mark().count() - start;
}

std::shared_ptr<hl2::CWindow> hl2::CBase_Scene::GetWindow() const noexcept
{
	return m_Window;
}

hl2::CSceneManager* hl2::CBase_Scene::GetSceneManager() const noexcept
{
	return m_SceneManager;
}

void hl2::CBase_Scene::SetSceneManager(hl2::CSceneManager* _Mgr) noexcept
{
	m_SceneManager = _Mgr;
}
