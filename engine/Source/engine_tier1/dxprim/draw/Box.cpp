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
					std::uniform_real_distribution<float>& _Bdist)
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

		struct PSLightConstants
		{
			DirectX::XMVECTOR pos;
		};

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		this->AddStaticBind(std::make_unique<engine::CInputLayout>(_Gfx, ied, pvsbc));
		this->AddStaticBind(std::make_unique<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		this->SetIndexFromStatic();
	}
	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, *this));
	DirectX::XMStoreFloat3x3(
		&m_Matrix,
		DirectX::XMMatrixScaling(1.0f, 1.0f, _Bdist(_Rng))
	);
}

void engine::CBox::Update(float _Dt) noexcept
{
	m_Positional.Roll += m_Speed.dRoll * _Dt;
	m_Positional.Pitch += m_Speed.dPitch * _Dt;
	m_Positional.Yaw += m_Speed.dYaw * _Dt;
	m_Positional.Theta += m_Speed.dTheta * _Dt;
	m_Positional.Phi += m_Speed.dPhi * _Dt;
	m_Positional.Chi += m_Speed.dChi * _Dt;
}

DirectX::XMMATRIX engine::CBox::GetTransformMatrix() const noexcept
{
	return DirectX::XMLoadFloat3x3(&m_Matrix) *
		DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Pitch, m_Positional.Yaw, m_Positional.Roll) *
		DirectX::XMMatrixTranslation(m_R, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Theta, m_Positional.Phi, m_Positional.Chi);
}
