#include "Topology.h"
#include <typeinfo>

engine::CPrim_Topology::CPrim_Topology(engine::CGraphicalOutput& _Gfx, D3D11_PRIMITIVE_TOPOLOGY _Type)
	: m_Type(_Type)
{}

void engine::CPrim_Topology::Bind(engine::CGraphicalOutput& _Gfx)
{
	GetContext(_Gfx)->IASetPrimitiveTopology(m_Type);
}

std::string engine::CPrim_Topology::Discriminate(D3D11_PRIMITIVE_TOPOLOGY _Type)
{
	using namespace std::string_literals;
	return typeid(engine::CPrim_Topology).name() + "#"s + std::to_string(_Type);
}

std::string engine::CPrim_Topology::GenerateDiscriminator() const noexcept
{
	return this->Discriminate(m_Type);
}