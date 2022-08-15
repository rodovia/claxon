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
	CBase_Scene(CBase_Scene& _Ref)
		: m_Window(_Ref.m_Window)
	{};
	CBase_Scene() = default;
	virtual ~CBase_Scene();
	float CreateFrame() noexcept;
	virtual void Start() 
	{};
	virtual void End() 
	{};

private:
	hl2::CBasicTimer m_Timer;
protected:
	std::shared_ptr<CWindow> m_Window;
protected:
	virtual void Render(float _Dt) = 0;
	std::shared_ptr<CWindow> GetWindow() const noexcept;
};

}
