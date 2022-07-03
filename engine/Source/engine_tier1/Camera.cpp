#include "Camera.h"
#include <imgui/imgui.h>

DirectX::XMMATRIX engine::CCamera::GetMatrix() const noexcept
{
	const auto pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet(0.0f, 0.0f, -m_R, 0.0f),
		DirectX::XMMatrixRotationRollPitchYaw(m_Positional.Phi, -m_Positional.Theta, 0.0f)
	);
	return DirectX::XMMatrixLookAtLH(
		pos, DirectX::XMVectorZero(),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * DirectX::XMMatrixRotationRollPitchYaw(
		m_Positional.Pitch,
		-m_Positional.Yaw,
		m_Positional.Roll
	);
}

void engine::CCamera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Câmara"))
	{
		ImGui::Text("Posição");
			ImGui::SliderFloat("R", &m_R, 0.0f, 80.0f, "%1.f");
			ImGui::SliderAngle("Theta", &m_Positional.Theta, -180.0f, 180.0f);
			ImGui::SliderAngle("Phi", &m_Positional.Phi, -89.0f, 89.0f);

		ImGui::Text("Orientação");
			ImGui::SliderAngle("Pitch", &m_Positional.Pitch, -180.0f, 180.f);
			ImGui::SliderAngle("Yaw", &m_Positional.Yaw, -180.0f, 180.f);
			ImGui::SliderAngle("Roll", &m_Positional.Roll, -180.0f, 180.f);

		if (ImGui::Button("Redefinir aos padrões"))
		{
			this->Reset();
		}
	}

	ImGui::End();
}

void engine::CCamera::Reset()
{
	m_R = 20.0f;
	m_Positional =
	{
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f
	};
}
