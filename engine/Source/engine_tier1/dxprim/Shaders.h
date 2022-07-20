/* -*- c++ -*- */
#pragma once

#include <string>

#include <engine_tier1/GraphicalOutput.h>
#include <engine_tier0/DLL.h>
#include <engine_tier0/BaseBind.h>

namespace engine
{

class  CVertexShader : public CBase_Bind
{
public:
	CVertexShader(CGraphicalOutput& _Gfx, std::wstring _Filename);
	void Bind(CGraphicalOutput&) override;
	ID3DBlob* GetBytecode() const noexcept;
private:
	CUtl_ComPtr<ID3DBlob> m_Blob;
};

class CPixelShader : public CBase_Bind
{
public:
	CPixelShader(CGraphicalOutput& _Gfx, std::wstring _Filename);
	void Bind(CGraphicalOutput&) override;
	ID3DBlob* GetBytecode() const noexcept;
private:
	CUtl_ComPtr<ID3DBlob> m_Blob;
};

}
