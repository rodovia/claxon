/* -*- c++ -*- */
#pragma once

#include <vector>
#include <engine_tier0/DLL.h>
#include <engine_tier1/GraphicalOutput.h>

#define _ENGINE_RESERVED_TAG "RESERVEDTAG_NOTINCODEX"

namespace engine
{

class CIndexBuffer : public CBase_Bind
{
public:
	CIndexBuffer(CGraphicalOutput& _Gfx, std::vector<unsigned short> _Indices)
		: CIndexBuffer(_Gfx, _Indices, _ENGINE_RESERVED_TAG)
	{}

	CIndexBuffer(CGraphicalOutput& _Gfx, std::vector<unsigned short> _Indices, std::string _Tag);
	~CIndexBuffer() {}
	void Bind(CGraphicalOutput& _Gfx) override;
	UINT GetCount() const noexcept;
	static std::string Discriminate(std::vector<unsigned short>, std::string _Tag) noexcept;
	std::string GenerateDiscriminator() const noexcept;
private:
	std::string m_Tag;
	std::vector<unsigned short> m_Indices;
	UINT m_IndicesCount;
	CUtl_ComPtr<ID3D11Buffer> m_IndexBuffer;
};

}
