#pragma once

#include <vector>
#include <DirectXMath.h>
#include <engine_tier1/BindableCodex.h>
#include <engine_tier1/VertexLayout.h>

namespace engine
{

class CIndexedTriangleList
{
public:
	CIndexedTriangleList() = default;
	CIndexedTriangleList(layout::CVertexBuffer _VertsIn, std::vector<unsigned short> _Indices)
		: m_Vertices(std::move(_VertsIn)),
		  m_Indices(std::move(_Indices))
	{
		// assert(m_Vertices.Size() > 2);
		assert(m_Indices.size() % 3 == 0);
	}

	void Transform(DirectX::FXMMATRIX _Matrix)
	{
		for (int i = 0; i < m_Vertices.Size(); i++)
		{
			auto& pos = m_Vertices[i].Attrib<layout::Position3D>();
			DirectX::XMStoreFloat3(
				&pos,
				DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), _Matrix)
			);
		}
	}
/*
	void SetNormalsIndependentFlat()
	{
		using namespace DirectX; // DirectX::XMVECTOR::operator- (binary)

		assert(m_Indices.size() % 3 == 0 && m_Indices.size() > 0);
		for (size_t i = 0; i < m_Indices.size(); i += 3)
		{
			auto& v0 = m_Vertices[m_Indices[i]];
			auto& v1 = m_Vertices[m_Indices[i + 1]];
			auto& v2 = m_Vertices[m_Indices[i + 2]];

			const auto p0 = DirectX::XMLoadFloat3(&v0.pos);
			const auto p1 = DirectX::XMLoadFloat3(&v1.pos);
			const auto p2 = DirectX::XMLoadFloat3(&v2.pos);

			const auto norm = DirectX::XMVector3Normalize(
				DirectX::XMVector3Cross(p1 - p0, p2 - p0));

			DirectX::XMStoreFloat3(&v0.norm, norm);
			DirectX::XMStoreFloat3(&v1.norm, norm);
			DirectX::XMStoreFloat3(&v2.norm, norm);
		}
	}
	*/
public:
	layout::CVertexBuffer m_Vertices;
	std::vector<unsigned short> m_Indices;
};

}
