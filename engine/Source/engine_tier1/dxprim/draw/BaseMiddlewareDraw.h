/* -*- c++ -*- */
#pragma once

#include "BaseDraw.h"
#include <engine_tier1/dxprim/IndexBuffer.h>

namespace engine
{

template<class T>
class CBase_MiddlewareDraw : public CBase_Draw
{
protected:
	static bool IsStaticInit()
	{
		return !m_StaticBinds.empty();
	}

	static void AddStaticBind(std::shared_ptr<CBase_Bind> _Bind)
	{
		m_StaticBinds.push_back(std::move(_Bind));
	}

	void AddStaticIndexBuffer(std::shared_ptr<CIndexBuffer> _Buffer) noexcept
	{
		std::printf("AddStaticIndexBuffer\n");
		m_IndexBuffer = _Buffer.get();
		m_StaticBinds.push_back(std::move(_Buffer));
	}

	void SetIndexFromStatic() noexcept
	{
		for (const auto& b : m_StaticBinds)
		{
			if (const auto p = dynamic_cast<CIndexBuffer*>(b.get()))
			{
				m_IndexBuffer = p;
				return;
			}
		}
	}
protected:
	const std::vector<std::shared_ptr<CBase_Bind>>& GetStaticBinds() const noexcept override
	{
		return m_StaticBinds;
	}
private:
	static std::vector<std::shared_ptr<CBase_Bind>> m_StaticBinds;
};

template<class T>
std::vector<std::shared_ptr<CBase_Bind>> CBase_MiddlewareDraw<T>::m_StaticBinds;

}

