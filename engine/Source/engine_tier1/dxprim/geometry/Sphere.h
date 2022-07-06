/* -*- c++ -*- */
#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <engine_tier0/_Math.h>

namespace engine
{

class CSphere
{
public:
	template<class T>
	static CIndexedTriangleList<T> MakeTessel(int _LatDivisions, int _LonDivisions)
	{
		assert(_LatDivisions >= 3);
		assert(_LonDivisions >= 3);

		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float latAngle = PI_f / _LatDivisions;
		const float lonAngle = 2.0f * PI_f / _LonDivisions;

		std::vector<T> vertices;
		for (int ila = 1; ila < _LatDivisions; ila++)
		{
			const auto labase = DirectX::XMVector3Transform(
				base, DirectX::XMMatrixRotationX(latAngle * ila)
			);
			for (int ilo = 0; ilo < _LonDivisions; ilo++)
			{
				vertices.emplace_back();
				auto v = DirectX::XMVector3Transform(
					labase,
					DirectX::XMMatrixRotationZ(((float) _LonDivisions) * ilo)
				);
				DirectX::XMStoreFloat3(&vertices.back().pos, v);
			}
		}

		const unsigned short nPole = vertices.size();
		vertices.emplace_back();
		DirectX::XMStoreFloat3(&vertices.back().pos, base);

		const unsigned short sPole = vertices.size();
		vertices.emplace_back();
		DirectX::XMStoreFloat3(&vertices.back().pos, DirectX::XMVectorNegate(base));

		const auto CalculateIndex = [_LatDivisions, _LonDivisions](unsigned short _Lat, unsigned short _Lon)
		{
			return _Lat * _LonDivisions + _Lon;
		};

		// vvvv Index calculation vvvv // ^^^^ Vertex calculation ^^^^
		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < _LatDivisions; iLat++)
		{
			for (unsigned short iLong = 0; iLong < _LonDivisions; iLong++)
			{
				indices.push_back(CalculateIndex(iLat, iLong));
				indices.push_back(CalculateIndex(iLat + 1, iLong));
				indices.push_back(CalculateIndex(iLat, iLong + 1));
				indices.push_back(CalculateIndex(iLat, iLong + 1));
				indices.push_back(CalculateIndex(iLat + 1, iLong));
				indices.push_back(CalculateIndex(iLat + 1, iLong + 1));
			}

			indices.push_back(CalculateIndex(iLat, _LonDivisions - 1));
			indices.push_back(CalculateIndex(iLat + 1, _LonDivisions - 1));
			indices.push_back(CalculateIndex(iLat, 0));
			indices.push_back(CalculateIndex(iLat, 0));
			indices.push_back(CalculateIndex(iLat + 1, _LonDivisions - 1));
			indices.push_back(CalculateIndex(iLat + 1, 0));
		}

		for (unsigned short iLong = 0; iLong < _LonDivisions; iLong++)
		{
			// north
			indices.push_back(nPole);
			indices.push_back(CalculateIndex(0, iLong));
			indices.push_back(CalculateIndex(0, iLong + 1));
			// south
			indices.push_back(CalculateIndex(_LatDivisions - 2, iLong + 1));
			indices.push_back(CalculateIndex(_LatDivisions - 2, iLong));
			indices.push_back(sPole);
		}

		// north
		indices.push_back(nPole);
		indices.push_back(CalculateIndex(0, _LonDivisions - 1));
		indices.push_back(CalculateIndex(0, 0));
		// south
		indices.push_back(CalculateIndex(_LatDivisions - 2, 0));
		indices.push_back(CalculateIndex(_LatDivisions - 2, _LonDivisions - 1));
		indices.push_back(sPole);

		return { std::move(vertices), std::move(indices) };
	}

	template<class T>
	static CIndexedTriangleList<T> Make()
	{
		return MakeTessel<T>(12, 24);
	}
};

}
