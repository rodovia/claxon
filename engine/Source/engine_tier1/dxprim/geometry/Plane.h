#pragma once

#include <array>
#include <engine_tier0/_Math.h>
#include "IndexedTriangleList.h"

namespace engine
{

class CPlane
{
public:
	template<class T>
	static CIndexedTriangleList<T> MakeTessel(int _DivisionsX, int _DivisionsY)
	{
		assert(_DivisionsX >= 1); 
		assert(_DivisionsY >= 1);

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const int nVertX = _DivisionsX + 1;
		const int nVertY = _DivisionsY + 1;
		std::vector<T> verts(nVertX * nVertY);
		{
			const float sideX = width / 2.0f;
			const float sideY = height / 2.0f;

			const float divSizeX = width / float(_DivisionsX);
			const float divSizeY = height / float(_DivisionsY);
			const auto bottomLeft = DirectX::XMVectorSet(-sideX, -sideY, 0.0f, 0.0f);

			for (int y = 0, i = 0; i < nVertY; y++)
			{
				const float yPos = float(y) * divSizeY;
				for (int x = 0; x < nVertX; x++, i++)
				{
					const auto v = DirectX::XMVectorAdd(
						bottomLeft, 
						DirectX::XMVectorSet(
							float(x) * divSizeX, 
							yPos, 
							0.0f, 0.0f)
					);
					DirectX::XMStoreFloat3(&verts[i].pos, v);
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(std::pow(_DivisionsX * _DivisionsY, 2) * 6);
		{
			const auto vxy2i = [nVertX](size_t x, size_t y)
			{
				return (unsigned short)(y * nVertX + x);
			};

			for (size_t y = 0; y < _DivisionsY; y++)
			{
				for (size_t x = 0; x < _DivisionsX; x++)
				{
					const std::array<unsigned short, 4> indarray =
					{
						vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x, y + 1), vxy2i(x + 1, y + 1),
					};
					indices.push_back(indarray[0]);
					indices.push_back(indarray[2]);
					indices.push_back(indarray[1]);
					indices.push_back(indarray[1]);
					indices.push_back(indarray[2]);
					indices.push_back(indarray[3]);
				}
			}
		}

		return { std::move(verts), std::move(indices) };
	}

	template<class V>
	static CIndexedTriangleList<V> Make()
	{
		return MakeTessel<V>(1, 1);
	}
};

}
