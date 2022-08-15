#pragma once

#include <tier0/DLL.h>
#include "BaseScene.h"
#include <memory>

namespace hl2
{

class DLLEXP CSceneManager
{
public:
	static CSceneManager& Instance()
	{
		static CSceneManager mgr;
		return mgr;
	}

	void ShowScene() const noexcept;
	void SetScene(std::shared_ptr<CBase_Scene> _Scene) noexcept;
	void SetReRender(bool _Flag) noexcept;

	static void ReloadScene(std::vector<std::string>);
private:
	CSceneManager() = default;
	bool m_ShouldReRender = true;
	std::shared_ptr<CBase_Scene> m_CurrentScene;
};

}
