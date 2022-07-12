/* -*- c++ -*- */
#pragma once

#include <random>
#include "BaseMiddlewareDraw.h"
#include <engine_tier0/BasePrimitiveObject.h>

namespace engine
{

class _ENGINE_DLLEXP CSkinnedBox : public CBase_PrimObject<CSkinnedBox>
{
public:
	CSkinnedBox(CGraphicalOutput& _Gfx,
		std::mt19937& _Rng,
		std::uniform_real_distribution<float>& _Adist,
		std::uniform_real_distribution<float>& _Ddist,
		std::uniform_real_distribution<float>& _Odist,
		std::uniform_real_distribution<float>& _Rdist,
		std::uniform_real_distribution<float>& _Bdist);
};

}
