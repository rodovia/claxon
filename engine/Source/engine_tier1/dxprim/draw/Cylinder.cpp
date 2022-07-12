#include "Cylinder.h"

#include <engine_tier1/dxprim/geometry/Prism.h>
#include <engine_tier1/Surface.h>
#include <engine_tier1/dxprim/InputLayout.h>
#include <engine_tier1/dxprim/Shaders.h>
#include <engine_tier1/dxprim/Topology.h>
#include <engine_tier1/dxprim/ConstantBuffer.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/dxprim/VertexBuffer.h>

engine::CCylinder::CCylinder(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
	std::uniform_real_distribution<float>& _Adist,
	std::uniform_real_distribution<float>& _Ddist,
	std::uniform_real_distribution<float>& _Odist,
	std::uniform_real_distribution<float>& _Rdist,
	std::uniform_real_distribution<float>& _Bdist,
	std::uniform_int_distribution<int>& _Tdist
)
	: CBase_PrimObject(_Gfx, _Rng, _Adist, _Ddist, _Odist, _Rdist, _Bdist)
{
	if (!this->IsStaticInit())
	{
	
		auto pvs = std::make_unique<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
		auto pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));

		this->AddStaticBind(std::make_unique<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("IndexPhong_PS.cso")));
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			_ENGINE_POSITION_IED,
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		this->AddStaticBind(std::make_unique<engine::CInputLayout>(_Gfx, ied, pvsbc));
		this->AddStaticBind(std::make_unique<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct  PSMaterialConstant
		{
			DirectX::XMFLOAT3A Colors[6] =
			{
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f },
				{ 1.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f }
			};
			float SpecularIntensity = 0.6f;
			float SpecularPower = 30.0f;
		} mtct;

		auto cbf = std::make_unique<engine::CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 10u);
		cbf->Update(_Gfx, mtct);
		this->AddStaticBind(std::move(cbf));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
	};
	auto model = engine::CPrism::MakeTesselatedIndependentCapNormals<Vertex>(_Tdist(_Rng));
	this->AddBind(std::make_unique<engine::CVertexBuffer>(_Gfx, model.m_Vertices));
	this->AddIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, model.m_Indices));

	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));
}
