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
	void End() override;
private:
	engine::CCamera m_Cam;
	std::optional<engine::CModel> m_Wall;
	engine::CLightPoint m_Light{GetWindow()->Graphics()};
	hl2::CBasicTimer m_Timer;
	bool m_ShowDemoWindow = true;

protected:
	void Render(float _Dt) override;
};

}
