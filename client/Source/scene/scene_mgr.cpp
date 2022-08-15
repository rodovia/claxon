#include "SceneManager.h"
#include <engine_ui/Console.h>

engine::CConCmd reload("reload", hl2::CSceneManager::ReloadScene);

void hl2::CSceneManager::SetScene(std::shared_ptr<CBase_Scene> _Scene) noexcept
{
	if (!_Scene)
	{
		return;
	}

	if (m_CurrentScene)
	{
		m_CurrentScene->End();
	}

	m_CurrentScene = _Scene;
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

class CBlanket_Scene : public hl2::CBase_Scene
{
public:
	CBlanket_Scene(std::shared_ptr<CBase_Scene> _NextScene)
		: m_NextScene(_NextScene),
		CBase_Scene(*_NextScene)
	{};

protected:
	void Render(float _Dt) override
	{
		// this->GetWindow()->Graphics().;
		if (ImGui::Begin("A Carregar"))
		{
			ImGui::Text("A carregar...");
			hl2::CSceneManager::Instance().SetScene(m_NextScene);
		}
		ImGui::End();
	};
private:
	std::shared_ptr<CBase_Scene> m_NextScene;
};

void hl2::CSceneManager::ReloadScene(std::vector<std::string> _Args)
{
	hl2::CSceneManager& mgr = CSceneManager::Instance();
	std::shared_ptr<CBase_Scene> uniq = mgr.m_CurrentScene;
	mgr.SetScene(std::make_shared<CBlanket_Scene>(uniq));
}