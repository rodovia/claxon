#pragma once

#include <string>

#include <engine_tier1/GraphicalOutput.h>
#include <engine_tier0/DLL.h>
#include <engine_tier0/BaseBind.h>

namespace engine
{

class _ENGINE_DLLEXP  CVertexShader : public CBase_Bind
{
public:
	CVertexShader(CGraphicalOutput& _Gfx, std::wstring _Filename);
	void Bind(CGraphicalOutput&) override;
	ID3DBlob* GetBytecode() const noexcept;
private:
	ComPtr<ID3DBlob> m_Blob;
};

class _ENGINE_DLLEXP CPixelShader : public CBase_Bind
{
public:
	CPixelShader(CGraphicalOutput& _Gfx, std::wstring _Filename);
	void Bind(CGraphicalOutput&) override;
	ID3DBlob* GetBytecode() const noexcept;
private:
	ComPtr<ID3DBlob> m_Blob;
};

}
