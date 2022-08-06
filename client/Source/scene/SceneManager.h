#pragma once

#include <tier0/DLL.h>
#include "BaseScene.h"
#include <memory>

namespace hl2
{

class DLLEXP CSceneManager
{
public:
	CSceneManager() = default;
	void ShowScene() const noexcept;
	void SetScene(std::unique_ptr<CBase_Scene> _Scene) noexcept;
	void SetReRender(bool _Flag) noexcept;
private:
	bool m_ShouldReRender = true;
	std::unique_ptr<CBase_Scene> m_CurrentScene;
};

}
