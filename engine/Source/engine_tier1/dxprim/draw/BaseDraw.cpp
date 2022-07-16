#include "BaseDraw.h"
#include <engine_tier0/BaseBind.h>

void engine::CBase_Draw::Draw(engine::CGraphicalOutput& _Gfx) const
{
	for (const std::shared_ptr<CBase_Bind>& b : m_Binds)
	{	
		b->Bind(_Gfx);
	}

	for (const std::shared_ptr<CBase_Bind>& b : GetStaticBinds())
	{
		b->Bind(_Gfx);
	}

	return _Gfx.DrawIndexed(m_IndexBuffer->GetCount());
}

void engine::CBase_Draw::AddBind(std::shared_ptr<engine::CBase_Bind> _Bind)
{
	m_Binds.push_back(std::move(_Bind));
}

void engine::CBase_Draw::AddIndexBuffer(std::shared_ptr<engine::CIndexBuffer> _Buffer) noexcept
{
	m_IndexBuffer = _Buffer.get();
	m_Binds.push_back(std::move(_Buffer));
}
