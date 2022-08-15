#include "MainMenuScene.h"
#include "GameScene.h"
#include <scene/SceneManager.h>

void hl2::CScn_MainMenu::Render(float _Dt)
{
	if (ImGui::Begin("Menu"))
	{
		if (ImGui::Button("Goto"))
		{
			CSceneManager::Instance().SetScene(std::make_shared<CScene_Game>(this->GetWindow()));
		}
	}
	ImGui::End();
}
