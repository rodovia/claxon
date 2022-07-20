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
	CTexture(CGraphicalOutput& _Gfx, const CSurface& _Surfc);
	void Bind(CGraphicalOutput& _Gfx) noexcept override;
protected:
	CUtl_ComPtr<ID3D11ShaderResourceView> m_TextureView;
};

}
