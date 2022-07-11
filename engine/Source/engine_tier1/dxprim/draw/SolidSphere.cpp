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

engine::CSolidSphere::CSolidSphere(CGraphicalOutput& _Gfx, float _Radius)
{
	if (!this->IsStaticInit())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
			float padding; // round to 16 bytes
		};

		auto model = CSphere::Make<Vertex>();
		model.Transform(DirectX::XMMatrixScaling(_Radius, _Radius, _Radius));
		this->AddBind(std::make_unique<engine::CVertexBuffer>(_Gfx, model.m_Vertices));
		this->AddIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, model.m_Indices));

		auto pvs = std::make_unique<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Solid_VS.cso"));
		auto pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));
		this->AddStaticBind(std::make_unique<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Solid_PS.cso")));
		
		PSColorConstant cconst;
		std::unique_ptr<engine::CConstantPixelBuffer<PSColorConstant>> cpb = std::make_unique<engine::CConstantPixelBuffer<PSColorConstant>>(_Gfx, 0u);
		cpb->Update(_Gfx, cconst);

		this->AddStaticBind(std::move(cpb));
		const std::vector<D3D11_INPUT_ELEMENT_DESC>  ied = {
			_ENGINE_POSITION_IED
		};
		this->AddStaticBind(std::make_unique<engine::CInputLayout>(_Gfx, ied, pvsbc));
		this->AddStaticBind(std::make_unique<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		this->SetIndexFromStatic();
	}

	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));
}

void engine::CSolidSphere::SetPos(DirectX::XMFLOAT3 _Pos) noexcept
{
	m_Pos = _Pos;
}

void engine::CSolidSphere::Update(float _Dt) noexcept
{}

DirectX::XMMATRIX engine::CSolidSphere::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
}