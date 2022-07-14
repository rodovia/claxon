#include "Shaders.h"
#include <d3dcompiler.h>

engine::CVertexShader::CVertexShader(engine::CGraphicalOutput& _Gfx, 
									std::wstring _Filename)
{
	D3DReadFileToBlob(_Filename.c_str(), &m_Blob);
}

ID3DBlob* engine::CVertexShader::GetBytecode() const noexcept
{
	return m_Blob.Get();
}

void engine::CVertexShader::Bind(engine::CGraphicalOutput& _Gfx)
{
	CUtl_ComPtr<ID3D11VertexShader> shader;
	GetDevice(_Gfx)->CreateVertexShader(m_Blob->GetBufferPointer(), 
										m_Blob->GetBufferSize(), 
										nullptr, &shader);
	GetContext(_Gfx)->VSSetShader(shader.Get(), nullptr, 0u);
}

engine::CPixelShader::CPixelShader(engine::CGraphicalOutput& _Gfx,
	std::wstring _Filename)
{
	D3DReadFileToBlob(_Filename.c_str(), &m_Blob);
}

ID3DBlob* engine::CPixelShader::GetBytecode() const noexcept
{
	return m_Blob.Get();
}

void engine::CPixelShader::Bind(engine::CGraphicalOutput& _Gfx)
{
	CUtl_ComPtr<ID3D11PixelShader> shader;
	GetDevice(_Gfx)->CreatePixelShader(m_Blob->GetBufferPointer(),
		m_Blob->GetBufferSize(),
		nullptr, &shader);
	GetContext(_Gfx)->PSSetShader(shader.Get(), nullptr, 0u);
}
