#include "SceneManager.h"

void hl2::CSceneManager::SetScene(std::unique_ptr<CBase_Scene> _Scene) noexcept
{
	if (!_Scene)
	{
		return;
	}

	if (m_CurrentScene)
	{
		m_CurrentScene->End();
		m_CurrentScene.release();
	}

	m_CurrentScene = std::move(_Scene);
	m_CurrentScene->SetSceneManager(this);
	m_CurrentScene->Start();
}

void hl2::CSceneManager::ShowScene() const noexcept
{
	m_CurrentScene->CreateFrame();
}

void hl2::CSceneManager::SetReRender(bool _Flag) noexcept
{
	m_ShouldReRender = _Flag;
}
