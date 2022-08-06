#pragma once

#include <tier0/DLL.h>
#include <scene/BaseScene.h>

namespace hl2
{

class DLLEXP CScn_MainMenu : public CBase_Scene
{
public:
	using CBase_Scene::CBase_Scene;

protected:
	void Render(float _Dt) override;
};

}
