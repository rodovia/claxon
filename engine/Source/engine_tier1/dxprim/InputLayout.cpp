#include "InputLayout.h"

engine::CInputLayout::CInputLayout(engine::CGraphicalOutput& _Gfx,
								const std::vector<D3D11_INPUT_ELEMENT_DESC>& _Layout,
								ID3DBlob* _BlobBytecode)
{
	GetDevice(_Gfx)->CreateInputLayout(
		_Layout.data(), (UINT) std::size(_Layout),
		_BlobBytecode->GetBufferPointer(),
		_BlobBytecode->GetBufferSize(),
		&m_InLayout
	);
}

void engine::CInputLayout::Bind(CGraphicalOutput& _Gfx)
{
	GetContext(_Gfx)->IASetInputLayout(m_InLayout.Get());
}
