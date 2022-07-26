/* -*- c++ -*- */
#pragma once

#include <engine_tier0/ComPtr.h>
#include <engine_tier0/BaseBind.h>

namespace engine
{

class CSurface;

class CTexture : public CBase_Bind
{
public:
	CTexture(CGraphicalOutput& _Gfx, const CSurface& _Surfc, unsigned int _Slot = 0);
	void Bind(CGraphicalOutput& _Gfx) noexcept override;
protected:
	unsigned int m_Slot;
	CUtl_ComPtr<ID3D11ShaderResourceView> m_TextureView;
};

}
