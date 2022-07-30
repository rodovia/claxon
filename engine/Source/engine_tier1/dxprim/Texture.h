/* -*- c++ -*- */
#pragma once

#include <engine_tier0/ComPtr.h>
#include <engine_tier0/BaseBind.h>
#include <string>

namespace engine
{

class CSurface;

class CTexture : public CBase_Bind
{
public:
	CTexture(CGraphicalOutput& _Gfx, const std::wstring& _Name, unsigned int _Slot = 0);
	void Bind(CGraphicalOutput& _Gfx) noexcept override;
	static std::string Discriminate(const std::wstring& _Name, unsigned int _Slot = 0) noexcept;
	std::string GenerateDiscriminator() noexcept override;
protected:
	unsigned int m_Slot;
	std::wstring m_Name;
	CUtl_ComPtr<ID3D11ShaderResourceView> m_TextureView;
};

}
