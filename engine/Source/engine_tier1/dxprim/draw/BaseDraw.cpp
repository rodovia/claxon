#include "BaseDraw.h"
#include <engine_tier0/BaseBind.h>
#include <engine_ui/Console.h>
#include <DirectXCollision.h>

engine::CConVar frustum_enabl("frustum_enabl", "1");

void engine::CBase_Draw::Draw(engine::CGraphicalOutput& _Gfx) const
{
	if (!this->IsOnFrustum(_Gfx))
	{
		return;
	}

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

	if (typeid(*_Bind) == typeid(engine::CTransformConstantBuffer))
	{
		assert(m_Cb == nullptr && "Cannot bind multiple transform constant buffers");
		m_Cb = &static_cast<engine::CTransformConstantBuffer&>(*_Bind);
	}
	m_Binds.push_back(std::move(_Bind));
}

void engine::CBase_Draw::SetPosition(DirectX::XMFLOAT3 _Pos)
{
	const_cast<engine::CBase_Draw*>(this)->m_Position = _Pos;
}

bool engine::CBase_Draw::IsOnFrustum(CGraphicalOutput& _Gfx) const
{
	using namespace DirectX;
	if (!frustum_enabl.GetBool())
	{
		return true;
	}
	engine::CTransformConstantBuffer::Transforms tf = m_Cb->GetTransforms(_Gfx);
	XMFLOAT4 fl4{0.0f,0.0f,0.0f,1.0f};
	XMVECTOR fl4v = XMLoadFloat4(&fl4);
	XMFLOAT3 globalScale;
	XMVECTOR gsv = XMMatrixDeterminant(tf.ModelView);
	XMStoreFloat3(&globalScale, gsv);

	XMVECTOR vec = XMVector4Transform(fl4v, tf.ModelView);
	const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);
	BoundingFrustum frus(_Gfx.GetProjection());

	// maxScale is the diameter of the sphere
	BoundingSphere sphr(m_Position, maxScale * 0.5f);
	return frus.Contains(sphr) == DISJOINT;
}
