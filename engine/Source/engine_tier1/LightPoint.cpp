#include "LightPoint.h"

engine::CLightPoint::CLightPoint(engine::CGraphicalOutput& _Gfx, float _Radius)
	: m_Mesh(_Gfx, _Radius),
	  m_Buf(_Gfx)
{}

void engine::CLightPoint::SpawnControlWindow()
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_Pos.x, -60.0f, 60.0f, "%2.f");
		ImGui::SliderFloat("Y", &m_Pos.y, -60.0f, 60.0f, "%2.f");
		ImGui::SliderFloat("Z", &m_Pos.z, -60.0f, 60.0f, "%2.f");
		
		if (ImGui::Button("Reset"))
		{
			this->Reset();
		}
	}

	ImGui::End();
}

void engine::CLightPoint::Reset() noexcept
{
	m_Pos = { 0.0f, 0.0f, 0.0f };
}

void engine::CLightPoint::Draw(CGraphicalOutput& _Gfx) noexcept
{
	m_Mesh.SetPos(m_Pos);
	m_Mesh.Draw(_Gfx);
}

void engine::CLightPoint::Bind(CGraphicalOutput& _Gfx) const noexcept
{
	m_Buf.Update(_Gfx, CLightPoint::CBLightPoint{ m_Pos });
	m_Buf.Bind(_Gfx);
}
