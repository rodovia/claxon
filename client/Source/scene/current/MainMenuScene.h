#pragma once

#include <tier0/DLL.h>
#include <scene/BaseScene.h>
#include <MountedPaths.h>
#include <engine_audio/SoundOutput.h>

namespace hl2
{

class DLLEXP CScn_MainMenu : public CBase_Scene
{
public:
	using CBase_Scene::CBase_Scene;

private:
	engine::CSoundOutput m_So;
	engine::CSound m_Snd{ &m_So, _GETPATH("resources/audio/Steam message alert.wav") };

protected:
	void Render(float _Dt) override;
};

}
