#include "Shaders.h"
#include <d3dcompiler.h>
#include <tier0lib/String0.h>
#include <engine_tier1/BindableCodex.h>

engine::CVertexShader::CVertexShader(engine::CGraphicalOutput& _Gfx, 
									std::wstring _Filename)
	: m_Path(_Filename)
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

std::string engine::CVertexShader::Discriminate(std::wstring_view _Key)
{
	using namespace std::string_literals;
	return typeid(engine::CVertexShader).name() + "#"s + tier0::ConvertToMultiByteString(std::wstring(_Key));
}

std::string engine::CVertexShader::GenerateDiscriminator() const noexcept
{
	return this->Discriminate(m_Path);
}

// engine::CPixelShader vvvv

engine::CPixelShader::CPixelShader(engine::CGraphicalOutput& _Gfx,
	std::wstring _Filename)
	: m_Path(_Filename)
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

std::string engine::CPixelShader::Discriminate(std::wstring_view _Key)
{
	using namespace std::string_literals;
	return typeid(engine::CPixelShader).name() + "#"s + tier0::ConvertToMultiByteString(std::wstring(_Key));
}

std::string engine::CPixelShader::GenerateDiscriminator() const noexcept
{
	return this->Discriminate(m_Path);
}
