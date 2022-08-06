#include <scene/BaseScene.h>

#include <engine_tier1/Surface.h>
#include <engine_tier1/LightPoint.h>
#include <engine_tier1/Camera.h>
#include <engine_tier1/Model.h>

namespace hl2
{

class DLLEXP CScene_Game : public CBase_Scene
{
public:
	using CBase_Scene::CBase_Scene;

	void Start() override;
private:
	engine::CCamera m_Cam;
	engine::CModel m_Wall{ GetWindow()->Graphics(), _GETPATH("resources/model/goblin/goblinx.obj") };
	engine::CLightPoint m_Light{GetWindow()->Graphics()};
	hl2::CBasicTimer m_Timer;
	bool m_ShowDemoWindow = true;

protected:
	void Render(float _Dt) override;
};

}
