#include "LightPoint.h"

engine::CLightPoint::CLightPoint(engine::CGraphicalOutput& _Gfx, float _Radius)
	: m_Mesh(_Gfx, _Radius),
	  m_Buf(_Gfx)
{
	this->Reset();
}

void engine::CLightPoint::SpawnControlWindow()
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_ConstBufferData.Position.x, -60.0f, 60.0f, "%2.f");
		ImGui::SliderFloat("Y", &m_ConstBufferData.Position.y, -60.0f, 60.0f, "%2.f");
		ImGui::SliderFloat("Z", &m_ConstBufferData.Position.z, -60.0f, 60.0f, "%2.f");
		
		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &m_ConstBufferData.DiffuseIntensity, -1.0f, 2.0f, "%.2f");
		ImGui::ColorEdit3("Diffuse", &m_ConstBufferData.DiffuseColor.x);
		ImGui::ColorEdit3("Ambient", &m_ConstBufferData.Ambient.x);

		ImGui::Text("Attenuation");
		ImGui::SliderFloat("Constant", &m_ConstBufferData.AttenuationConst, 0.05f, 10.0f, "%.2f");
		ImGui::SliderFloat("Linear", &m_ConstBufferData.AttenuationLinear, 0.0001f, 4.0f, "%.4f");
		ImGui::SliderFloat("Quadratic", &m_ConstBufferData.AttenuationQuadratic, 0.0000001f, 10.0f, "%.7f");

		if (ImGui::Button("Reset"))
		{
			this->Reset();
		}
	}

	ImGui::End();
}

void engine::CLightPoint::Reset() noexcept
{
	m_ConstBufferData = { 
		{ 0.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.5f },
		{ 1.0f, 1.0f, 1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f
	};
}

void engine::CLightPoint::Draw(CGraphicalOutput& _Gfx) noexcept
{
	m_Mesh.SetPos(m_ConstBufferData.Position);
	m_Mesh.Draw(_Gfx);
}

void engine::CLightPoint::Bind(CGraphicalOutput& _Gfx, DirectX::FXMMATRIX _View) noexcept
{
	auto dataCopy = m_ConstBufferData;
	const auto pos = DirectX::XMLoadFloat3(&m_ConstBufferData.Position);
	DirectX::XMStoreFloat3(&dataCopy.Position, DirectX::XMVector3Transform(pos, _View));
	m_Buf.Update(_Gfx, dataCopy);
	m_Buf.Bind(_Gfx);
}
