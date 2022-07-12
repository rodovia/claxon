#include "Pyramid.h"

#include <array>

#include <engine_tier1/Surface.h>
#include <engine_tier1/dxprim/VertexBuffer.h>
#include <engine_tier1/dxprim/InputLayout.h>
#include <engine_tier1/dxprim/ConstantBuffer.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/dxprim/Topology.h>
#include <engine_tier1/dxprim/Shaders.h>
#include <engine_tier1/dxprim/geometry/Cone.h>

engine::CPyramid::CPyramid(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
	std::uniform_real_distribution<float>& _Adist,
	std::uniform_real_distribution<float>& _Ddist,
	std::uniform_real_distribution<float>& _Odist,
	std::uniform_real_distribution<float>& _Rdist,
	std::uniform_real_distribution<float>& _Bdist,
	std::uniform_int_distribution<int>& _Tdist)
	: CBase_PrimObject(_Gfx, _Rng, _Adist, _Ddist, _Odist, _Rdist, _Bdist)
{
	if (!this->IsStaticInit())
	{
		auto pvs = std::make_unique<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("BlendPhong_VS.cso"));
		auto pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));

		this->AddStaticBind(std::make_unique<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("BlendPhong_PS.cso")));
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			_ENGINE_POSITION_IED,
			{ "Color",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		this->AddStaticBind(std::make_unique<engine::CInputLayout>(_Gfx, ied, pvsbc));
		this->AddStaticBind(std::make_unique<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			float SpecularIntensity = 0.6f;
			float SpecularPower = 30.0f;
			float Padding[2];
		} colorc;
		auto m = std::make_unique<engine::CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 10u);
		m->Update(_Gfx, colorc);
		this->AddStaticBind(std::move(m));

	}

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 norm;
		std::array<char, 4> color;
		char _unused;
	};

	int tessel = _Tdist(_Rng);
	engine::CIndexedTriangleList<Vertex> model = CCone::MakeTesselatedIndependentFaces<Vertex>(tessel);

	for (auto& i : model.m_Vertices)
	{
		i.color = { (char)10, (char)10, (char)255 };
	}
	for (int i = 0; i < tessel; i++)
	{
		model.m_Vertices[i * 3].color = { (char)255, (char)10, (char)10 };
	}

	model.m_Vertices.front().color = { (char)255, (char)20, (char)20 };

	model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 0.7f));
	model.SetNormalsIndependentFlat();
	this->AddBind(std::make_unique<engine::CVertexBuffer>(_Gfx, model.m_Vertices));
	this->AddIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, model.m_Indices));


	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));
}
