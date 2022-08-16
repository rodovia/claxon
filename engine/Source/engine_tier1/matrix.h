#pragma once

#include <engine_tier0/DLL.h>
#include <DirectXMath.h>

namespace engine
{

_ENGINE_DLLEXP DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& _Matrix);

_ENGINE_DLLEXP DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& _Matrix);

}
