#include "TestPlane.h"

#include <engine_tier1/dxprim/geometry/Plane.h>
#include <engine_tier1/VertexLayout.h>
#include <engine_tier1/BindableCodex.h>

#include <engine_tier1/dxprim/fwd.h>

#include <engine_tier1/Surface.h>

#define __hl2_repeat(_F) ((float*)({(float)_F, (float)_F, (float)_F, (float)_F}))


engine::CPlane::CPlane(CGraphicalOutput& _Gfx, float _Size, DirectX::XMFLOAT4 _Color)
{
	m_Pmc.Color = _Color;
	CIndexedTriangleList tl = CPrim_Plane::Make();
	tl.Transform(DirectX::XMMatrixScaling(_Size, _Size, _Size));
	const std::string tag = "?plane$" + std::to_string(_Size);

	this->AddBind(CCodex::Query<CVertexBuffer>(_Gfx, tag, tl.m_Vertices));
	this->AddBind(CCodex::Query<CIndexBuffer>(_Gfx, tl.m_Indices, tag));

	this->AddBind(CCodex::Query<CPixelShader>(_Gfx, MAKE_SHADER_RESOURCE("Texture_PS.cso")));

	std::shared_ptr<CVertexShader> vs = CCodex::Query<CVertexShader>(_Gfx, MAKE_SHADER_RESOURCE("Texture_VS.cso"));
	ID3DBlob* blob = vs->GetBytecode();
	this->AddBind(std::move(vs));

	this->AddBind(CCodex::Query<CInputLayout>(_Gfx, tl.m_Vertices.Layout(), blob));
	auto buf = CCodex::Query<CConstantPixelBuffer<PSConstantBuffer>>(_Gfx, 1u);
	buf->Update(_Gfx, m_Pmc);
	this->AddBind(std::move(buf));
	this->AddBind(CCodex::Query<CTexture>(_Gfx, _GETPATH("resources/textures/cat.png")));

	this->AddBind(CCodex::Query<CPrim_Topology>(_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	this->AddBind(CCodex::Query<CSampler>(_Gfx));
	this->AddBind(std::make_shared<CTransformConstantBuffer>(_Gfx, this, 0u));
	this->AddBind(CCodex::Query<engine::CBlender>(_Gfx, true, std::optional<std::array<float, 4>>({1.0f, 1.0f, 1.0f, 1.0f})));
	this->AddBind(CCodex::Query<engine::CRasterizer>(_Gfx, true));
}

void engine::CPlane::SetPos(DirectX::XMFLOAT3 _Pos) noexcept
{
	m_Pos = _Pos;
}

void engine::CPlane::SetRotation(DirectX::XMFLOAT3 _NewRot) noexcept
{
	m_Rotation = _NewRot;
}

DirectX::XMMATRIX engine::CPlane::GetTransformMatrix(const CCamera& _Cam) const noexcept
{
/*
	return DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y,
												 m_Rotation.z)
		   * DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
		   */
	engine::_Pair pr = _Cam.GetPitchYaw();
	return DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z) * 
		DirectX::XMMatrixRotationRollPitchYaw(pr.x, m_Rotation.y, m_Rotation.z);
}

void engine::CPlane::SpawnTestWindow(CGraphicalOutput& _Gfx) noexcept
{
	if (ImGui::Begin("Plane"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_Pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_Pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_Pos.z, -80.0f, 80.0f, "%.1f");

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &m_Rotation.x, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &m_Rotation.y, -180.0f, 180.0f);
		ImGui::SliderAngle("Roll", &m_Rotation.z, -180.0f, 180.0f);

		ImGui::Text("Shading");
		bool changedInt = ImGui::ColorEdit4("Color", &m_Pmc.Color.x);
		if (changedInt)
		{
			CCodex::QueryExistent<engine::CConstantPixelBuffer<PSConstantBuffer>>()->Update(_Gfx, m_Pmc);
		}
	}

	ImGui::End();
}
