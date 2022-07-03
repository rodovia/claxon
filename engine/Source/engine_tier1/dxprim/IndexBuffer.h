#pragma once

#include <vector>
#include <engine_tier0/DLL.h>
#include <engine_tier1/GraphicalOutput.h>

namespace engine
{

class _ENGINE_DLLEXP CIndexBuffer : public CBase_Bind
{
public:
	CIndexBuffer(CGraphicalOutput& _Gfx, std::vector<unsigned short> _Indices);
	~CIndexBuffer() {}
	void Bind(CGraphicalOutput& _Gfx) override;
	UINT GetCount() const noexcept;
private:
	UINT m_IndicesCount;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
};

}
