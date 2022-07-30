/* -*- c++ -*- */
#pragma once

#include <engine_tier0/DLL.h>
#include <engine_tier1/GraphicalOutput.h>

namespace engine
{

class CPrim_Topology : public CBase_Bind
{
public:
	CPrim_Topology(CGraphicalOutput& _Gfx, D3D11_PRIMITIVE_TOPOLOGY _Type);
	void Bind(CGraphicalOutput& _Gfx) override;
	static std::string Discriminate(D3D11_PRIMITIVE_TOPOLOGY _Type);
	std::string GenerateDiscriminator() const noexcept;
private:
	D3D11_PRIMITIVE_TOPOLOGY m_Type;
};

}
