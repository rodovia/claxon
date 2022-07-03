#pragma once

#include "DXGIInfoManager.h"
#include "DLL.h"
#include <d3d11.h>

namespace engine
{

class CGraphicalOutput;

class _ENGINE_DLLEXP CBase_Bind
{
public:
	CBase_Bind() = default;
	virtual void Bind(CGraphicalOutput& _Gfx) = 0;
	virtual ~CBase_Bind() = default;
	CBase_Bind(CBase_Bind&) = default;
protected:
	static ID3D11DeviceContext* GetContext(CGraphicalOutput& _Gfx) noexcept;
	static ID3D11Device* GetDevice(CGraphicalOutput& _Gfx) noexcept;
	static engine::CDXGIInfoManager& GetInfoManager(CGraphicalOutput& _Gfx);
};

}
