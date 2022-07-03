#include "Pyramid.h"

#include <engine_tier1/Surface.h>
#include <engine_tier1/dxprim/VertexBuffer.h>
#include <engine_tier1/dxprim/InputLayout.h>
#include <engine_tier1/dxprim/TransformConstBuffer.h>
#include <engine_tier1/dxprim/Topology.h>
#include <engine_tier1/dxprim/Shaders.h>
#include <engine_tier1/dxprim/geometry/Cone.h>

engine::CPyramid::CPyramid(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
	std::uniform_real_distribution<float>& _Adist,
	std::uniform_real_distribution<float>& _Ddist,
	std::uniform_real_distribution<float>& _Odist,
	std::uniform_real_distribution<float>& _Rdist)
	: m_Speed({
		_Ddist(_Rng), /* dRoll */
		_Ddist(_Rng), /* dPitch */
		_Ddist(_Rng), /* dYaw */
		_Odist(_Rng), /* dTheta */
		_Odist(_Rng), /* dPhi */
		_Odist(_Rng), /* dChi */
	}),
	m_Positional({
		0.0f,		  /* Roll */
		0.0f,		  /* Pitch */
		0.0f,		  /* Yaw */
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
				unsigned char r, g, b, a;
			} color;
		};

		auto model = CCone::MakeTesselated<Vertex>(4);
		model.m_Vertices[0].color = { 255, 255, 0 };
		model.m_Vertices[1].color = { 255, 255, 0 };
		model.m_Vertices[2].color = { 255, 255, 0 };
		model.m_Vertices[3].color = { 255, 255, 0 };
		model.m_Vertices[4].color = { 255, 255, 80 };
		model.m_Vertices[5].color = { 255, 10, 0 };

		model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 0.7f));
		this->AddStaticBind(std::make_unique<engine::CVertexBuffer>(_Gfx, model.m_Vertices));

		auto pvs = std::make_unique<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("ColorBlend_VS.cso"));
		auto pvsbc = pvs->GetBytecode();
		this->AddStaticBind(std::move(pvs));

		this->AddStaticIndexBuffer(std::make_unique<engine::CIndexBuffer>(_Gfx, model.m_Indices));
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
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

void engine::CPyramid::Update(float _Dt) noexcept
{
	m_Positional.Roll += m_Speed.dRoll * _Dt;
	m_Positional.Pitch += m_Speed.dPitch * _Dt;
	m_Positional.Yaw += m_Speed.dYaw * _Dt;
	m_Positional.Theta += m_Speed.dTheta * _Dt;
	m_Positional.Phi += m_Speed.dPhi * _Dt;
	m_Positional.Chi += m_Speed.dChi * _Dt;
}

DirectX::XMMATRIX engine::CPyramid::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Pitch, m_Positional.Yaw, m_Positional.Roll) *
		DirectX::XMMatrixTranslation(m_R, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Theta, m_Positional.Phi, m_Positional.Chi);
}
