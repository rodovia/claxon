#include "matrix.h"
#include <cmath>

DirectX::XMFLOAT3 engine::ExtractEulerAngles(const DirectX::XMFLOAT4X4& _Matrix)
{
	DirectX::XMFLOAT3 euler;
	euler.x = std::asinf(-_Matrix._32);

	if (std::cosf(euler.x) > 0.001)
	{
		euler.y = std::atan2f(_Matrix._31, _Matrix._33);
		euler.y = std::atan2f(_Matrix._12, _Matrix._22);
	}
	else
	{
		euler.y = 0.0f;
		euler.z = std::atan2f(-_Matrix._21, _Matrix._11);
	}

	return euler;
}

DirectX::XMFLOAT3 engine::ExtractTranslation(const DirectX::XMFLOAT4X4& _Matrix)
{
	return {_Matrix._41, _Matrix._42, _Matrix._43};
}
