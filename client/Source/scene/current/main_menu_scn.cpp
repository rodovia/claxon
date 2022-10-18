#include "MainMenuScene.h"
#include "GameScene.h"
#include <scene/SceneManager.h>
#include <scene/current/FrogScene.h>

void hl2::CScn_MainMenu::Render(float _Dt)
{
	static bool frogSelected, sponzaSelected;
	static const char* current = nullptr;
	auto wnd = this->GetWindow();
	if (ImGui::Begin("Menu"))
	{
		if (ImGui::Button("Pulo do sapo"))
		{
			CSceneManager::Instance().SetScene(std::make_shared<CFrogScene>(wnd));
			ImGui::End();
			return;
		}

		if (ImGui::Button("Cenário Sponza"))
		{
			CSceneManager::Instance().SetScene(std::make_shared<CScene_Game>(wnd));
			ImGui::End();
			return;
		}

		if (ImGui::Button("Iniciar"))
		{
			
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
