#include "Box.h"

#include <engine_tier1/dxprim/geometry/Sphere.h>
#include <engine_tier1/dxprim/geometry/Cube.h>

#include <engine_tier1/dxprim/VertexBuffer.h>
#include <engine_tier1/dxprim/ConstantBuffer.h>
#include <engine_tier1/Surface.h>				// MAKE_SHADER_RESOURCE
#include <engine_tier1/dxprim/Shaders.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/dxprim/InputLayout.h>
#include <engine_tier1/dxprim/Topology.h>

engine::CBox::CBox(CGraphicalOutput& _Gfx, 
					std::mt19937& _Rng,
					std::uniform_real_distribution<float>& _Adist,
					std::uniform_real_distribution<float>& _Ddist,
					std::uniform_real_distribution<float>& _Odist,
					std::uniform_real_distribution<float>& _Rdist,
					std::uniform_real_distribution<float>& _Bdist,
	                DirectX::XMFLOAT3 _Material
	)
	: CBase_PrimObject(_Gfx, _Rng, _Adist, _Ddist, _Odist, _Rdist, _Bdist)
{
	if (!this->IsStaticInit())
	{

		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 norm;
		};

		auto model = engine::CCube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));

		this->AddStaticBind(std::make_unique<engine::CVertexBuffer>(_Gfx, model.m_Vertices));

		auto pvs = std::make_shared<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
		auto pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));
		this->AddStaticBind(std::make_unique<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_PS.cso")));
		this->AddStaticIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, model.m_Indices));
		
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			_ENGINE_POSITION_IED,
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		this->AddStaticBind(std::make_unique<engine::CInputLayout>(_Gfx, ied, pvsbc));
		this->AddStaticBind(std::make_unique<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		this->SetIndexFromStatic();
	}
	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));

	struct PSMaterialConstant
	{
		alignas(16) DirectX::XMFLOAT3 Color;
		float SpecularIntensity = 0.6f;
		float SpecularPower = 30.0f;
		float _unused[2];
	} colorc;
	colorc.Color = _Material;
	auto ma = std::make_unique<engine::CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 1u);
	ma->Update(_Gfx, colorc);

	this->AddBind(std::move(ma));

	DirectX::XMStoreFloat3x3(
		&m_Matrix,
		DirectX::XMMatrixScaling(1.0f, 1.0f, _Bdist(_Rng))
	);
}

DirectX::XMMATRIX engine::CBox::GetTransformMatrix() const noexcept
{
	return DirectX::XMLoadFloat3x3(&m_Matrix) * CBase_PrimObject::GetTransformMatrix();
}
