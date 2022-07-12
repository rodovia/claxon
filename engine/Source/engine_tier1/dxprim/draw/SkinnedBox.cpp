#include "SkinnedBox.h"

#include <memory>

#include <engine_tier1/Surface.h>
#include <engine_tier1/dxprim/geometry/Cube.h>
#include <engine_tier1/dxprim/Topology.h>
#include <engine_tier1/dxprim/InputLayout.h>
#include <engine_tier1/dxprim/IndexBuffer.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/dxprim/geometry/Plane.h>
#include <engine_tier1/dxprim/Texture.h>
#include <engine_tier1/dxprim/Sampler.h>
#include <engine_tier1/dxprim/Shaders.h>
#include <engine_tier1/dxprim/VertexBuffer.h>

engine::CSkinnedBox::CSkinnedBox(CGraphicalOutput& _Gfx,
	std::mt19937& _Rng,
	std::uniform_real_distribution<float>& _Adist,
	std::uniform_real_distribution<float>& _Ddist,
	std::uniform_real_distribution<float>& _Odist,
	std::uniform_real_distribution<float>& _Rdist,
	std::uniform_real_distribution<float>& _Bdist
	)	: CBase_PrimObject(_Gfx, _Rng, _Adist, _Ddist, _Odist, _Rdist, _Bdist)
{
	if (!this->IsStaticInit())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 norm;
			DirectX::XMFLOAT2 tc;
		};

		auto surf = MAKE_SURFACE_MOUNT("resources/textures/cube.png");
		auto model = engine::CCube::MakeIndependentTextured<Vertex>();
		model.SetNormalsIndependentFlat();

		this->AddStaticBind(std::make_unique<engine::CTexture>(_Gfx, surf));
		this->AddStaticBind(std::make_unique<engine::CVertexBuffer>(_Gfx, model.m_Vertices));
		this->AddStaticBind(std::make_unique<engine::CSampler>(_Gfx));
		this->AddStaticBind(std::make_unique<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("TexturePhong_PS.cso")));

		auto pvs = std::make_unique<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("TexturePhong_VS.cso"));
		auto pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));

		this->AddStaticIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, model.m_Indices));
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			_ENGINE_POSITION_IED,
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		this->AddStaticBind(std::make_unique<engine::CInputLayout>(_Gfx, ied, pvsbc));
		this->AddStaticBind(std::make_unique<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			float SpecularIntensity = 0.6f;
			float SpecularPower = 30.0f;
			float _unused[2];
		} colorc;
		auto m = std::make_unique<engine::CConstantPixelBuffer<PSMaterialConstant>>(_Gfx, 10u);
		m->Update(_Gfx, colorc);
		this->AddStaticBind(std::move(m));
	}
	else
	{
		this->SetIndexFromStatic();
	}

	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));
}
