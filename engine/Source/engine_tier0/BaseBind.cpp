#include "BaseBind.h"
#include <engine_tier1/GraphicalOutput.h>

ID3D11DeviceContext* engine::CBase_Bind::GetContext(engine::CGraphicalOutput& _Gfx) noexcept
{
	return _Gfx.m_Context.Get();
}

ID3D11Device* engine::CBase_Bind::GetDevice(engine::CGraphicalOutput& _Gfx) noexcept
{
	return _Gfx.m_Device.Get();
}

engine::CDXGIInfoManager& engine::CBase_Bind::GetInfoManager(engine::CGraphicalOutput& _Gfx)
{
	return _Gfx.m_InfoQueue;
}
