#pragma once

#include "GraphicalOutput.h"

namespace hl2
{

class CBase_Bind
{
public:
	virtual void Bind(hl2::CGraphicalOutput& gfx) = 0;
	virtual ~CBase_Bind() = default;
protected:
	static ID3D11DeviceContext* Context(hl2::CGraphicalOutput& gfx) noexcept;
	static ID3D11Device* Device(hl2::CGraphicalOutput& gfx) noexcept;
};

}
