#include "Rasterizer.h"
#include <engine_ui/Console.h>
#include <typeinfo>

engine::CConVar mat_wireframe("mat_wireframe", "0");

engine::CRasterizer::CRasterizer(CGraphicalOutput& _Gfx, bool _RenderBack)
	: m_RenderBack(_RenderBack)
{
	D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	rdesc.CullMode = _RenderBack ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	rdesc.FillMode = mat_wireframe.GetBool() ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	GetDevice(_Gfx)->CreateRasterizerState(&rdesc, &m_Rstate);
}

void engine::CRasterizer::Bind(CGraphicalOutput& _Gfx)
{
	GetContext(_Gfx)->RSSetState(m_Rstate.Get());
}

std::string engine::CRasterizer::GenerateDiscriminator() noexcept
{
	return this->Discriminate(m_RenderBack);
}

std::string engine::CRasterizer::Discriminate(bool _RenderBack) noexcept
{
	std::string fmt = typeid(engine::CRasterizer).name();
	return fmt + (_RenderBack ? "_R" : "_");
}
