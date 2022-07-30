#include "InputLayout.h"

engine::CInputLayout::CInputLayout(engine::CGraphicalOutput& _Gfx,
	layout::CVertexLayout _Layout,
	ID3DBlob* _BlobBytecode)
	: m_Layout(std::move(_Layout))
{
	const std::vector<D3D11_INPUT_ELEMENT_DESC> eld = m_Layout.D3DLayout();

	GetDevice(_Gfx)->CreateInputLayout(
		eld.data(), (UINT) std::size(eld),
		_BlobBytecode->GetBufferPointer(),
		_BlobBytecode->GetBufferSize(),
		&m_InLayout
	);
}

void engine::CInputLayout::Bind(CGraphicalOutput& _Gfx)
{
	GetContext(_Gfx)->IASetInputLayout(m_InLayout.Get());
}

std::string engine::CInputLayout::Discriminate(layout::CVertexLayout _Layout,
	ID3DBlob* _BlobBytecode)
{
	using namespace std::string_literals;
	return typeid(engine::CInputLayout).name() + "#"s + _Layout.Mangle();
}

std::string engine::CInputLayout::GenerateDiscriminator() const noexcept
{
	return this->Discriminate(m_Layout, nullptr);
}
