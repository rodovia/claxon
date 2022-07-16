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
	template<class T>
	friend class CBase_MiddlewareDraw;
public:
	CBase_Draw() = default;
	CBase_Draw(const CBase_Draw&) = delete;

	void Draw(CGraphicalOutput& _Gfx) const;
protected:
	void AddBind(std::shared_ptr<CBase_Bind> _Bind);
	void AddIndexBuffer(std::shared_ptr<engine::CIndexBuffer> _Buffer) noexcept;
public:
	virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;
	virtual void Update(float _Dt) noexcept
	{};
	virtual ~CBase_Draw() = default;
private:
	virtual const std::vector<std::shared_ptr<CBase_Bind>>& GetStaticBinds() const noexcept = 0;
private:
	const CIndexBuffer* m_IndexBuffer = nullptr;
	std::vector<std::shared_ptr<CBase_Bind>> m_Binds;
};

}

