#pragma once

#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <engine_tier0/_Math.h>
#include <optional>
#include <engine_tier1/VertexLayout.h>

namespace engine
{

#pragma warning(disable : 4267) // 'initizaling': conversion from 'size_t' to 'unsigned short'; possible data loss.

	class CSphere
	{
	public:
		static CIndexedTriangleList MakeTessel(layout::CVertexLayout _Layout, int _LatDivisions, int _LonDivisions)
		{
			assert(_LatDivisions >= 3);
			assert(_LonDivisions >= 3);

			constexpr float radius = 1.0f;
			const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
			const float latAngle = PI_f / _LatDivisions;
			const float lonAngle = 2.0f * PI_f / _LonDivisions;

			layout::CVertexBuffer vertices{ std::move(_Layout) };
			for (int ila = 1; ila < _LatDivisions; ila++)
			{
				const auto labase = DirectX::XMVector3Transform(
					base, DirectX::XMMatrixRotationX(latAngle * ila)
				);
				for (int ilo = 0; ilo < _LonDivisions; ilo++)
				{
					DirectX::XMFLOAT3 calculatedPos;
					auto v = DirectX::XMVector3Transform(
						labase,
						DirectX::XMMatrixRotationZ(((float)_LonDivisions) * ilo)
					);
					DirectX::XMStoreFloat3(&calculatedPos, v);
					vertices.EmplaceBack(calculatedPos);
				}
			}

			const unsigned short nPole = (unsigned short) vertices.Size();
			{
				DirectX::XMFLOAT3 northpos;
				DirectX::XMStoreFloat3(&northpos, base);
				vertices.EmplaceBack(northpos);
			}

			const unsigned short sPole = (unsigned short)vertices.Size();
			{
				DirectX::XMFLOAT3 southpos;
				DirectX::XMStoreFloat3(&southpos, DirectX::XMVectorNegate(base));
				vertices.EmplaceBack(southpos);
			}
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

		static CIndexedTriangleList Make(std::optional<layout::CVertexLayout> _Layout = std::nullopt)
		{
			if (!_Layout)
			{
				_Layout = layout::CVertexLayout{}.Append(layout::Position3D);
			}

			return MakeTessel(*_Layout, 12, 24);
		}
	};

#pragma warning(pop)

}