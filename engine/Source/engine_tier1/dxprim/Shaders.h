/* -*- c++ -*- */
#pragma once

#include <string>

#include <engine_tier1/GraphicalOutput.h>
#include <engine_tier0/DLL.h>
#include <memory>
#include <engine_tier0/BaseBind.h>

namespace engine
{

class  CVertexShader : public CBase_Bind
{
public:
	CVertexShader(CGraphicalOutput& _Gfx, std::wstring _Filename);
	void Bind(CGraphicalOutput&) override;
	ID3DBlob* GetBytecode() const noexcept;
	static std::string Discriminate(std::wstring_view _Path);
	std::string GenerateDiscriminator() const noexcept override;
private:
	std::wstring m_Path;
	CUtl_ComPtr<ID3DBlob> m_Blob;
};

class CPixelShader : public CBase_Bind
{
public:
	CPixelShader(CGraphicalOutput& _Gfx, std::wstring _Filename);
	void Bind(CGraphicalOutput&) override;
	ID3DBlob* GetBytecode() const noexcept;
	static std::string Discriminate(std::wstring_view _Path);
	std::string GenerateDiscriminator() const noexcept override;
private:
	CUtl_ComPtr<ID3DBlob> m_Blob;
	std::wstring m_Path;
};

}
