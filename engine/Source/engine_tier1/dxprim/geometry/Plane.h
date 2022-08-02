#pragma once

#include <array>
#include <optional>
#include <engine_tier0/_Math.h>
#include "IndexedTriangleList.h"
#include <engine_tier1/VertexLayout.h>

namespace engine
{

class CPrim_Plane
{
public:
	static CIndexedTriangleList MakeTessel(layout::CVertexLayout _Layout,
										   int _DivisionsX, int _DivisionsY)
	{
		namespace dx = DirectX;
		assert(_DivisionsX >= 1);
		assert(_DivisionsY >= 1);

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const int nVertices_x = _DivisionsX + 1;
		const int nVertices_y = _DivisionsY + 1;
		layout::CVertexBuffer vb{std::move(_Layout)};

		{
			const float side_x = width / 2.0f;
			const float side_y = height / 2.0f;
			const float divisionSize_x = width / float(_DivisionsX);
			const float divisionSize_y = height / float(_DivisionsY);
			const float divisionSize_x_tc = 1.0f / float(_DivisionsX);
			const float divisionSize_y_tc = 1.0f / float(_DivisionsY);

			for (int y = 0, i = 0; y < nVertices_y; y++)
			{
				const float y_pos = float(y) * divisionSize_y - 1.0f;
				const float y_pos_tc = 1.0f - float(y) * divisionSize_y_tc;
				for (int x = 0; x < nVertices_x; x++, i++)
				{
					const float x_pos = float(x) * divisionSize_x - 1.0f;
					const float x_pos_tc = float(x) * divisionSize_x_tc;
					vb.EmplaceBack(dx::XMFLOAT3{x_pos, y_pos, 0.0f},
								   dx::XMFLOAT3{0.0f, 0.0f, -1.0f},
								   dx::XMFLOAT2{x_pos_tc, y_pos_tc});
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(Square(_DivisionsX * _DivisionsY) * 6);
		{
			const auto vxy2i = [nVertices_x](size_t x, size_t y)
			{ return (unsigned short)(y * nVertices_x + x); };
			for (size_t y = 0; y < _DivisionsY; y++)
			{
				for (size_t x = 0; x < _DivisionsX; x++)
				{
					const std::array<unsigned short, 4> indexArray = {
						vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x, y + 1),
						vxy2i(x + 1, y + 1)};
					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}

		return {std::move(vb), std::move(indices)};
	}

	static CIndexedTriangleList Make()
	{

		layout::CVertexLayout la = layout::CVertexLayout{}
									   .Append(layout::Position3D)
									   .Append(layout::Normal)
									   .Append(layout::Texture2D);
		return MakeTessel(la, 1, 1);
	}
};

} // namespace engine
