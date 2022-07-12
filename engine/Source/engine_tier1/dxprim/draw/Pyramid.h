/* -*- c++ -*- */
#pragma once

#include <random>
#include <engine_tier0/DLL.h>
#include <engine_tier0/BasePrimitiveObject.h>

namespace engine
{

class _ENGINE_DLLEXP CPyramid : public CBase_PrimObject<CPyramid>
{
public:
	CPyramid(CGraphicalOutput& _Gfx, std::mt19937& _Rng,
		std::uniform_real_distribution<float>& _Adist,
		std::uniform_real_distribution<float>& _Ddist,
		std::uniform_real_distribution<float>& _Odist,
		std::uniform_real_distribution<float>& _Rdist,
		std::uniform_real_distribution<float>& _Bdist,
		std::uniform_int_distribution<int>& _Tdist
	);
};

}
