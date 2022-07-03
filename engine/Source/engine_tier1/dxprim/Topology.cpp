#include "Topology.h"

engine::CPrim_Topology::CPrim_Topology(engine::CGraphicalOutput& _Gfx, D3D11_PRIMITIVE_TOPOLOGY _Type)
	: m_Type(_Type)
{}

void engine::CPrim_Topology::Bind(engine::CGraphicalOutput& _Gfx)
{
	GetContext(_Gfx)->IASetPrimitiveTopology(m_Type);
}
