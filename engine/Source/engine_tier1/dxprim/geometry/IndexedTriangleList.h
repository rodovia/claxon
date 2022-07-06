/* -*- c++ -*- */
#pragma once

#include <vector>
#include <DirectXMath.h>

namespace engine
{

template<class T>
class CIndexedTriangleList
{
public:
	CIndexedTriangleList() = default;
	CIndexedTriangleList(std::vector<T> _VertsIn, std::vector<unsigned short> _Indices)
		: m_Vertices(_VertsIn),
		  m_Indices(_Indices)
	{
		assert(_VertsIn.size() > 2);
		assert(_Indices.size() % 3 == 0);
	}

	void Transform(DirectX::FXMMATRIX _Matrix)
	{
		for (auto& v : m_Vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(
				&v.pos,
				DirectX::XMVector3Transform(pos, _Matrix)
			);
		}
	}

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
public:
	std::vector<T> m_Vertices;
	std::vector<unsigned short> m_Indices;
};

}
