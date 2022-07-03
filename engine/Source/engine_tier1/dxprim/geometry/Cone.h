#pragma once

#include "IndexedTriangleList.h"
#include <engine_tier0/_Math.h>

namespace engine
{

class CCone
{
public:
	template<class V>
	static CIndexedTriangleList<V> MakeTesselated( int _LongDivisions )
	{
		namespace dx = DirectX;
		assert(_LongDivisions >= 3 );

		const auto base = dx::XMVectorSet( 1.0f,0.0f,-1.0f,0.0f );
		const float longitudeAngle = 2.0f * PI_f / _LongDivisions;

		// base vertices
		std::vector<V> vertices;
		for( int iLong = 0; iLong < _LongDivisions; iLong++ )
		{
			vertices.emplace_back();
			auto v = dx::XMVector3Transform( 
				base,
				dx::XMMatrixRotationZ( longitudeAngle * iLong )
			);
			dx::XMStoreFloat3( &vertices.back().pos,v );
		}
		// the center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto iCenter = (unsigned short)(vertices.size() - 1);
		// the tip :darkness:
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,1.0f };
		const auto iTip = (unsigned short)(vertices.size() - 1);

		
		// base indices
		std::vector<unsigned short> indices;
		for( unsigned short iLong = 0; iLong < _LongDivisions; iLong++ )
		{
			indices.push_back( iCenter );
			indices.push_back( (iLong + 1) % _LongDivisions);
			indices.push_back( iLong );
		}

		// cone indices
		for( unsigned short iLong = 0; iLong < _LongDivisions; iLong++ )
		{
			indices.push_back( iLong );
			indices.push_back( (iLong + 1) % _LongDivisions);
			indices.push_back( iTip );
		}

		return { std::move( vertices ),std::move( indices ) };
	}
	template<class V>
	static CIndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>( 24 );
	}
};
}
