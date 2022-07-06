/* -*- c++ -*- */
#pragma once

#include <engine_tier0/BaseBind.h>
#include <wrl.h>

namespace engine
{

class CSurface;

class _ENGINE_DLLEXP CTexture : public CBase_Bind
{
public:
	CTexture(CGraphicalOutput& _Gfx, const CSurface& _Surfc);
	void Bind(CGraphicalOutput& _Gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
};

}
