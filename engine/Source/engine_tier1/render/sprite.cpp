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
	engine::CIndexedTriangleList lt = CPrim_Plane::Make();
	lt.Transform(DirectX::XMMatrixScaling(_Desc.Scale, _Desc.Scale, 1.0f));

	std::string fn = tier0::ConvertToMultiByteString(_Filename);
	this->AddBind(CCodex::Query<engine::CVertexBuffer>(_Gfx, "spr_" + fn, lt.m_Vertices));
	this->AddBind(CCodex::Query<engine::CIndexBuffer>(_Gfx, lt.m_Indices, "spr_" + fn));

	std::shared_ptr<engine::CVertexShader> vs = CCodex::Query<engine::CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Texture_VS.cso"));
	ID3DBlob* blob = vs->GetBytecode();
	this->AddBind(std::move(vs));

	this->AddBind(CCodex::Query<engine::CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Texture_PS.cso")));
	this->AddBind(CCodex::Query<engine::CInputLayout>(_Gfx, lt.m_Vertices.Layout(), blob));
	this->AddBind(CCodex::Query<engine::CSampler>(_Gfx));
	this->AddBind(CCodex::Query<engine::CTexture>(_Gfx, _Filename));
	this->AddBind(CCodex::Query<engine::CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	this->AddBind(std::make_unique<engine::CTransformConstantBuffer>(_Gfx, this));
	this->AddBind(CCodex::Query<engine::CBlender>(_Gfx, true, std::optional<std::array<float, 4>>({0.5f, 0.5f, 0.5f, 0.5f})));
	this->AddBind(CCodex::Query<engine::CRasterizer>(_Gfx, true));
}

DirectX::XMMATRIX engine::CSprite::GetTransformMatrix(const CCamera& _Cam) const noexcept
{
	engine::_Pair pr = _Cam.GetPitchYaw();
	return DirectX::XMMatrixTranslation(m_Pos.Position.x, m_Pos.Position.y, m_Pos.Position.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(pr.x, m_Pos.Rotation.y, m_Pos.Rotation.z);
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
