#include "BaseDraw.h"
#include <engine_tier0/BaseBind.h>

void engine::CBase_Draw::Draw(engine::CGraphicalOutput& _Gfx) const
{
	for (const std::shared_ptr<CBase_Bind>& b : m_Binds)
	{	
		b->Bind(_Gfx);
	}

	return _Gfx.DrawIndexed(m_IndexBuffer->GetCount());
}

void engine::CBase_Draw::AddBind(std::shared_ptr<engine::CBase_Bind> _Bind)
{
	if (typeid(*_Bind) == typeid(engine::CIndexBuffer))
	{
		assert(m_IndexBuffer == nullptr && "An attempt was made to bind mutiple index buffers");
		m_IndexBuffer = &static_cast<engine::CIndexBuffer&>(*_Bind);
	}
	m_Binds.push_back(std::move(_Bind));
}

