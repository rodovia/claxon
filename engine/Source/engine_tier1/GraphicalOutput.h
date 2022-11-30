/* -*- c++ -*- */
#pragma once

#include <tier0lib/Win32.h>
#include <engine_tier0/BaseBind.h>
#include <engine_tier0/DXGIInfoManager.h>

#include <imguihlp.h>

#include <d3d11.h>
#include <DirectXMath.h>
#include <engine_tier0/ComPtr.h>
#include <engine_tier1/Camera.h>

template<class T>
using ComPtr [[deprecated]] = engine::CUtl_ComPtr<T>;

namespace engine
{

class _ENGINE_DLLEXP CGraphicalOutput
{
	friend class CBase_Bind;
public:
	CGraphicalOutput(HWND hWnd, ImGuiContext* _Ctx = nullptr);
	CGraphicalOutput(const CGraphicalOutput&) = delete;
	CGraphicalOutput& operator=(const CGraphicalOutput&) = delete;
	~CGraphicalOutput();

	void ToggleImGui() noexcept;
	bool ImGuiEnabled() const noexcept;

	void BeginFrame(float _R, float _G, float _B);
	void BeginFrameNorm(unsigned char _R, unsigned char _G, unsigned char _B);
	void EndFrame();
	
	void SetViewport(int _Width, int _Height) noexcept;
	void DrawIndexed(UINT _Count);

	void SetFullScreen(bool _State);
	bool IsFullScreen();

	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetProjection(DirectX::FXMMATRIX _Proj) noexcept;
	CCamera& GetCamera() noexcept;
	void SetCamera(engine::CCamera _Cam) noexcept;
	D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept;

private:
	bool m_HasImGui = true;
	bool m_PendingEndFrameCall = false;

	CUtl_ComPtr<ID3D11Device> m_Device;
	CUtl_ComPtr<ID3D11DeviceContext> m_Context;
	CUtl_ComPtr<IDXGISwapChain> m_Swap;
	CUtl_ComPtr<ID3D11RenderTargetView> m_Target;
	CUtl_ComPtr<ID3D11DepthStencilView> m_DepthStencil;
	DirectX::XMMATRIX m_Projection;
	engine::CCamera m_Camera;
	D3D_FEATURE_LEVEL m_FeatLevel;
	engine::CDXGIInfoManager m_InfoQueue;
};

}