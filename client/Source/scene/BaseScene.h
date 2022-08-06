#pragma once

#include <tier0/DLL.h>
#include <tier1/Window.hh>
#include <tier0/Timer.h>

namespace hl2
{

class DLLEXP CBase_Scene
{
	friend class CSceneManager;

public:
	CBase_Scene(std::shared_ptr<CWindow> _Window);
	CBase_Scene() = default;
	~CBase_Scene();
	float CreateFrame() noexcept;
	virtual void Start() 
	{};
	virtual void End() 
	{};

private:
	hl2::CBasicTimer m_Timer;
	std::shared_ptr<CWindow> m_Window;
	class CSceneManager* m_SceneManager;

	void SetSceneManager(class CSceneManager* _Mgr) noexcept;

protected:
	virtual void Render(float _Dt) = 0;
	std::shared_ptr<CWindow> GetWindow() const noexcept;
	class CSceneManager* GetSceneManager() const noexcept;
};

}
