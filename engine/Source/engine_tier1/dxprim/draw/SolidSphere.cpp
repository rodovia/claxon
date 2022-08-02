#include "SolidSphere.h"

#include <engine_tier1/dxprim/geometry/Sphere.h>
#include <engine_tier1/dxprim/VertexBuffer.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/dxprim/InputLayout.h>
#include <engine_tier1/dxprim/Shaders.h>
#include <engine_tier1/dxprim/Topology.h>
#include <engine_tier1/dxprim/ConstantBuffer.h>
#include <engine_tier1/dxprim/IndexBuffer.h>
#include <engine_tier1/Surface.h>
#include <engine_tier1/VertexLayout.h>

#include <engine_tier1/BindableCodex.h>

engine::CSolidSphere::CSolidSphere(CGraphicalOutput& _Gfx, float _Radius)
{
	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float padding; // round to 16 bytes
	};

	engine::CIndexedTriangleList model = CSphere::Make(layout::CVertexLayout().Append(layout::Position3D));
	model.Transform(DirectX::XMMatrixScaling(_Radius, _Radius, _Radius));
	this->AddBind(CCodex::Query<engine::CVertexBuffer>(_Gfx, "SOLIDSPHERE", model.m_Vertices));
	this->AddBind(CCodex::Query<engine::CIndexBuffer>(_Gfx, model.m_Indices, "SOLIDSPHERE"));

	auto pvs = CCodex::Query<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Solid_VS.cso"));
	auto pvsbc = pvs->GetBytecode();
	this->AddBind(std::move(pvs));
	this->AddBind(CCodex::Query<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Solid_PS.cso")));

	PSColorConstant cconst;
	auto cpb = CCodex::Query<engine::CConstantPixelBuffer<PSColorConstant>>(_Gfx, 1u);
	cpb->Update(_Gfx, cconst);

	this->AddBind(std::move(cpb));
	this->AddBind(CCodex::Query<engine::CInputLayout>(_Gfx, model.m_Vertices.Layout(), pvsbc));
	this->AddBind(CCodex::Query<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));
}

void engine::CSolidSphere::SetPos(DirectX::XMFLOAT3 _Pos) noexcept
{
	m_Pos = _Pos;
}

DirectX::XMMATRIX engine::CSolidSphere::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
}