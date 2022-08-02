#include "TestPlane.h"

#include <engine_tier1/dxprim/geometry/Plane.h>
#include <engine_tier1/VertexLayout.h>
#include <engine_tier1/BindableCodex.h>

#include <engine_tier1/dxprim/fwd.h>

#include <engine_tier1/Surface.h>

engine::CPlane::CPlane(CGraphicalOutput& _Gfx, float _Size)
{
	CIndexedTriangleList tl = CPrim_Plane::Make();
	tl.Transform(DirectX::XMMatrixScaling(_Size, _Size, 1.0f));
	const std::string tag = "?plane$" + std::to_string(_Size);

	this->AddBind(CCodex::Query<CVertexBuffer>(_Gfx, tag, tl.m_Vertices));
	this->AddBind(CCodex::Query<CIndexBuffer>(_Gfx, tl.m_Indices, tag));

	const std::string texBase = "resources/textures/urban_misc/";
	this->AddBind(
		CCodex::Query<CTexture>(_Gfx, _GETPATH(texBase + "brickwall.jpg")));
	this->AddBind(CCodex::Query<CTexture>(
		_Gfx, _GETPATH(texBase + "brickwall_normal.jpg"), 1u));
	this->AddBind(CCodex::Query<CPixelShader>(
		_Gfx, MAKE_SHADER_RESOURCE("PhongNormalMap_PS.cso")));

	std::shared_ptr<CVertexShader> vs = CCodex::Query<CVertexShader>(
		_Gfx, MAKE_SHADER_RESOURCE("Phong_VS.cso"));
	ID3DBlob* blob = vs->GetBytecode();
	this->AddBind(std::move(vs));

	this->AddBind(
		CCodex::Query<CInputLayout>(_Gfx, tl.m_Vertices.Layout(), blob));
	auto buf = CCodex::Query<CConstantPixelBuffer<PSConstantBuffer>>(_Gfx, 10u);
	buf->Update(_Gfx, m_Pmc);
	this->AddBind(std::move(buf));
	this->AddBind(CCodex::Query<CPrim_Topology>(
		_Gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	this->AddBind(std::make_shared<CTransformConstantBuffer>(_Gfx, *this));
}

void engine::CPlane::SetPos(DirectX::XMFLOAT3 _Pos) noexcept
{
	m_Pos = _Pos;
}

void engine::CPlane::SetRotation(DirectX::XMFLOAT3 _NewRot) noexcept
{
	m_Rotation = _NewRot;
}

DirectX::XMMATRIX engine::CPlane::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y,
												 m_Rotation.z)
		   * DirectX::XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
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
		bool changedInt = ImGui::SliderFloat(
			"Specular Int.", &m_Pmc.SpecularIntensity, 0.0f, 100.0f);
		bool changedPower = ImGui::SliderFloat(
			"Specular Pwr.", &m_Pmc.SpecularPower, 0.0f, 100.0f);
		bool normaMapState = m_Pmc.EnableNormalMap == TRUE;
		bool cnd = ImGui::Checkbox("Enable Normal Map", &normaMapState);
		if (changedInt || changedPower || cnd)
		{
			CCodex::QueryExistent<
				engine::CConstantPixelBuffer<PSConstantBuffer>>()
				->Update(_Gfx, m_Pmc);
		}
	}

	ImGui::End();
}
