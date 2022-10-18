#include "Camera.h"
#include <imgui/imgui.h>
#include <engine_tier0/_Math.h>
#include <algorithm>

engine::CCamera::CCamera() noexcept
{
	this->Reset();
}

DirectX::XMMATRIX engine::CCamera::GetMatrix() const noexcept
{
	using namespace DirectX; // XMVECTOR::operator+ (binary)

	const DirectX::XMVECTOR fwdbv = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	auto lookVector = DirectX::XMVector3Transform(fwdbv,
		DirectX::XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f));

	auto camp = DirectX::XMLoadFloat3(&m_Position);
	const auto camt = camp + lookVector;
	return DirectX::XMMatrixLookAtLH(camp, camt, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void engine::CCamera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Câmara"))
	{
		ImGui::Text("Posição");
			ImGui::SliderFloat("X", &m_Position.x, -80.0f, 80.0f, "%1.f");
			ImGui::SliderAngle("Y", &m_Position.y, -80.0f, 80.0f);
			ImGui::SliderAngle("Z", &m_Position.z, -80.0f, 80.0f);

		ImGui::Text("Orientação");
			ImGui::SliderAngle("Yaw", &m_Yaw, -90.0f, 90.f);
			ImGui::SliderAngle("Pitch", &m_Pitch, 0.995f * -90.0f, 0.995f * 90.f);

		if (ImGui::Button("Redefinir aos padrões"))
		{
			this->Reset();
		}
	}

	ImGui::End();
}

void engine::CCamera::Reset()
{
	m_Position = { 0.0f, 7.5f, -18.0f };
	m_Pitch = 0.0f;
	m_Yaw = 0.0f;
}

void engine::CCamera::Rotate(float _DeltaX, float _DeltaY) noexcept
{
	m_Yaw = engine::WrapAngle(m_Yaw + _DeltaX * s_RotationSpeed);
	m_Pitch = std::clamp(m_Pitch + _DeltaY * s_RotationSpeed, 0.995f * -PI_f / 2.0f, 0.995f * PI_f / 2.0f);
}

void engine::CCamera::Translate(DirectX::XMFLOAT3 _Position) noexcept
{
	DirectX::XMStoreFloat3(&_Position, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&_Position),
		DirectX::XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f) *
		DirectX::XMMatrixScaling(s_TravelSpeed, s_TravelSpeed, s_TravelSpeed)
	));
	m_Position = {
		m_Position.x + _Position.x,
		m_Position.y + _Position.y,
		m_Position.z + _Position.z,
	};
}

struct engine::_Pair engine::CCamera::GetPitchYaw() const noexcept
{
	return { m_Pitch, m_Yaw };
}

DirectX::XMFLOAT3 engine::CCamera::GetPosition() const noexcept
{
	return m_Position;
}
