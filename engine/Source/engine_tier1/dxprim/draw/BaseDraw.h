/* -*- c++ -*- */
#pragma once

#include <memory>
#include <vector>

#include <engine_tier0/DLL.h>

#include <engine_tier1/dxprim/IndexBuffer.h>
#include <engine_tier1/GraphicalOutput.h>
#include <DirectXMath.h>

namespace engine
{

class _ENGINE_DLLEXP CBase_Draw
{
public:
	CBase_Draw() = default;
	CBase_Draw(const CBase_Draw&) = delete;

	void Draw(CGraphicalOutput& _Gfx) const;
protected:
	void AddBind(std::shared_ptr<CBase_Bind> _Bind);
public:
	virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;
	virtual ~CBase_Draw() = default;
private:
	const CIndexBuffer* m_IndexBuffer = nullptr;
	std::vector<std::shared_ptr<CBase_Bind>> m_Binds;
};

}

