// =============
// BaseDraw.h -- Base class for all drawables
// =============

#pragma once

#include <memory>
#include <vector>

#include <engine_tier0/DLL.h>

#include <engine_tier1/dxprim/IndexBuffer.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/GraphicalOutput.h>
#include <DirectXMath.h>

#include <engine_tier1/Camera.h>

namespace engine
{

class CTransformConstantBuffer;  /*	
								  * In TransformConstBuffer.h, this file gets included BEFORE
								  *	CTransformConstantBuffer is actually defined. We are lucky
								  *	because we only use it as a ptr.
								  */

bool ShouldDraw(CGraphicalOutput& _Gfx, DirectX::XMFLOAT3 _Position);

class _ENGINE_DLLEXP CBase_Draw
{
public:
	CBase_Draw() = default;
	CBase_Draw(const CBase_Draw&) = delete;

	void Draw(CGraphicalOutput& _Gfx) const;

protected:
	void AddBind(std::shared_ptr<CBase_Bind> _Bind);
	void SetPosition(DirectX::XMFLOAT3 _Position);
	bool m_ShouldDraw = true;

public:
	bool IsOnFrustum(CGraphicalOutput& _Gfx) const;
	virtual DirectX::XMMATRIX GetTransformMatrix(const CCamera&) const noexcept = 0;
	virtual ~CBase_Draw() = default;

private:
	DirectX::XMFLOAT3 m_Position;
	const CIndexBuffer* m_IndexBuffer = nullptr;
	const CTransformConstantBuffer* m_Cb = nullptr;
	std::vector<std::shared_ptr<CBase_Bind>> m_Binds;
};

} // namespace engine
