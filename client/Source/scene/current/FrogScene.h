#pragma once

#include <scene/BaseScene.h>
#include <engine_tier1/render/Sprite.h>
#include <engine_tier1/render/Model.h>
#include <engine_tier1/dxprim/draw/TestPlane.h>
#include <tier0/Timer.h>
#include <engine_tier1/Camera.h>
#include <MountedPaths.h>
#include <optional>

namespace hl2
{

class CFrogScene : public CBase_Scene
{
public:
	using CBase_Scene::CBase_Scene;

	void Start() override;

private:
	std::optional<engine::CSprite> m_Spr;
	std::optional<engine::CModel> m_Mdl;
	engine::CCamera m_Cam;
	hl2::CBasicTimer m_Timer;
	std::optional<engine::CPlane> m_Sph;

protected:
	void Render(float _Dt) override;
};

}
