#include "MainMenuScene.h"
#include "GameScene.h"
#include <scene/SceneManager.h>

void hl2::CScn_MainMenu::Render(float _Dt)
{
	auto wnd = this->GetWindow();
	if (ImGui::Begin("Menu"))
	{
		if (ImGui::Button("Goto"))
		{
			CSceneManager::Instance().SetScene(std::make_shared<CScene_Game>(wnd));
			
		}
	}

	if (wnd->m_Keyboard.IsKeyPressed('S'))
	{
		m_Snd.Play();
	}

	if (wnd->m_Keyboard.IsKeyPressed('K'))
	{
		m_Snd.Play();
	}

	ImGui::End();
}
