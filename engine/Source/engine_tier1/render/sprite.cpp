#include "Sprite.h"

#include <engine_tier1/dxprim/fwd.h>
#include <engine_tier1/dxprim/geometry/Plane.h>
#include <engine_tier1/VertexLayout.h>

#include <memory>
#include <tier0lib/String0.h>

#include <vector>

engine::CSprite::CSprite(CGraphicalOutput& _Gfx, std::wstring _Filename, SPRITE_DESCRIPTOR _Desc) noexcept
	: m_Pos({_Desc.InitialPos, {0.0f, 0.0f, 0.0f}})
{
	CIndexedTriangleList tl = CPrim_Plane::Make();
	this->AddBind(std::make_shared<CVertexBuffer>(_Gfx, "", tl.m_Vertices));
	this->AddBind(std::make_shared<CIndexBuffer>(_Gfx, tl.m_Indices, ""));

	this->AddBind(CCodex::Query<CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Texture_PS.cso")));

	std::shared_ptr<CVertexShader> vs = CCodex::Query<CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Sprite_VS.cso"));
	ID3DBlob* blob = vs->GetBytecode();
	this->AddBind(std::move(vs));

	this->AddBind(CCodex::Query<CInputLayout>(_Gfx, tl.m_Vertices.Layout(), blob));
	this->AddBind(CCodex::Query<CTexture>(_Gfx, _Filename));

	this->AddBind(CCodex::Query<CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	this->AddBind(CCodex::Query<CSampler>(_Gfx));
	this->AddBind(std::make_shared<CTransformConstantBuffer>(_Gfx, this, 0u));
	this->AddBind(CCodex::Query<engine::CBlender>(_Gfx, true, std::optional<std::array<float, 4>>({1.0f, 1.0f, 1.0f, 1.0f})));
	this->AddBind(CCodex::Query<engine::CRasterizer>(_Gfx, true));
}

DirectX::XMMATRIX engine::CSprite::GetTransformMatrix(const CCamera& _Cam) const noexcept
{
	engine::_Pair pr = _Cam.GetPitchYaw();
	return DirectX::XMMatrixTranslation(m_Pos.Position.x, m_Pos.Position.y, m_Pos.Position.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(pr.x, pr.y, m_Pos.Rotation.z);
}

void engine::CSprite::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Sprite"))
	{
		ImGui::SliderFloat3("Pos", &m_Pos.Position.x, 0.0f, 300.0f, "%.2f");
		ImGui::SliderFloat3("Rot", &m_Pos.Rotation.x, 0.0f, 300.0f, "%.2f");
		ImGui::End();
	}
}

void engine::CSprite::SetPos(DirectX::XMFLOAT3 _Np) noexcept
{
	m_Pos.Position = _Np;
}

void engine::CSprite::SetRotation(DirectX::XMFLOAT3 _Nr) noexcept
{
	m_Pos.Rotation = _Nr;
}
