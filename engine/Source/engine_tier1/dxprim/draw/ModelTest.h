#pragma once

#include <engine_tier0/BasePrimitiveObject.h>
#include <engine_tier0/DLL.h>

namespace engine
{
class _ENGINE_DLLEXP CModelTest : public CBase_PrimObject<CModelTest>
{
public:

	CModelTest(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
		std::uniform_real_distribution<float>& _Adist,
		std::uniform_real_distribution<float>& _Ddist,
		std::uniform_real_distribution<float>& _Odist,
		std::uniform_real_distribution<float>& _Rdist,
		std::uniform_real_distribution<float>& _Bdist,
		DirectX::XMFLOAT3 _Material,
		float _Scale);
};
}