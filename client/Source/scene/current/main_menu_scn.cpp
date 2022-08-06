#include "MainMenuScene.h"
#include "GameScene.h"
#include <scene/SceneManager.h>

void hl2::CScn_MainMenu::Render(float _Dt)
{
	if (ImGui::Begin("Menu"))
	{
		if (ImGui::Button("Goto"))
		{
			CSceneManager* mg = this->GetSceneManager();
			mg->SetScene(std::make_unique<CScene_Game>(this->GetWindow()));
		}
	}
	ImGui::End();
}
