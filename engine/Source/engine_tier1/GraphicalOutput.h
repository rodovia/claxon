#pragma once

#include <tier0lib/Win32.h>
#include <engine_tier0/BaseBind.h>
#include <engine_tier0/DXGIInfoManager.h>

#include <imguihlp.h>

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

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

	void DrawIndexed(UINT _Count);

	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetProjection(DirectX::FXMMATRIX _Proj) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	void SetCamera(DirectX::FXMMATRIX _Cam) noexcept;
	D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept;
private:
	bool m_HasImGui = true;
	bool m_PendingEndFrameCall = false;

	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_Context;
	ComPtr<IDXGISwapChain> m_Swap;
	ComPtr<ID3D11RenderTargetView> m_Target;
	ComPtr<ID3D11DepthStencilView> m_DepthStencil;	
	DirectX::XMMATRIX m_Projection;
	DirectX::XMMATRIX m_Camera;
	D3D_FEATURE_LEVEL m_FeatLevel;
	engine::CDXGIInfoManager m_InfoQueue;
};

}