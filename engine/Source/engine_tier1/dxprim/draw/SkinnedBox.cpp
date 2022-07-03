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
				std::uniform_real_distribution<float>& _Rdist)
	: m_Speed({
		_Ddist(_Rng),	/* dRoll */
		_Ddist(_Rng),	/* dPitch */
		_Ddist(_Rng),	/* dYaw */
		_Odist(_Rng),	/* dTheta */
		_Odist(_Rng),	/* dPhi */
		_Odist(_Rng),	/* dChi */
	}),
	m_Positional({
		0.0f,			/* Roll */
		0.0f,			/* Pitch */
		0.0f,			/* Yaw */
		_Adist(_Rng), /* Theta */
		_Adist(_Rng), /* Phi */
		_Adist(_Rng), /* Chi */
	}),
	m_R(_Rdist(_Rng))
{
	if (!this->IsStaticInit())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			struct
			{
				float u, v;
			} tex;
		};

		auto surf = MAKE_SURFACE_MOUNT("resources/textures/cube.png");
		auto model = engine::CCube::MakeSkinned<Vertex>();
		surf.WriteToFile(L"../../../mod/resources/textures/_test.bmp");

		this->AddStaticBind(std::make_unique<engine::CTexture>(_Gfx, surf));
		this->AddStaticBind(std::make_unique<engine::CVertexBuffer>(_Gfx, model.m_Vertices));
		this->AddStaticBind(std::make_unique<engine::CSampler>(_Gfx));
		this->AddStaticBind(std::make_unique<engine::CPixelShader>(_Gfx, _GETPATH("resources/hlsl/Texture_PS.cso")));

		auto pvs = std::make_unique<engine::CVertexShader>(_Gfx, _GETPATH("resources/hlsl/Texture_VS.cso"));
		auto pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));

		this->AddStaticIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, model.m_Indices));
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void engine::CSkinnedBox::Update(float _Dt) noexcept
{
	m_Positional.Roll += m_Speed.dRoll * _Dt;
	m_Positional.Pitch += m_Speed.dPitch * _Dt;
	m_Positional.Yaw += m_Speed.dYaw * _Dt;
	m_Positional.Theta += m_Speed.dTheta * _Dt;
	m_Positional.Phi += m_Speed.dPhi * _Dt;
	m_Positional.Chi += m_Speed.dChi * _Dt;
}

DirectX::XMMATRIX engine::CSkinnedBox::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Pitch, m_Positional.Yaw, m_Positional.Roll) *
		DirectX::XMMatrixTranslation(m_R, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Theta, m_Positional.Phi, m_Positional.Chi);
}
