#include "Blender.h"
#include <engine_tier0/Exceptions.h>
#include <typeinfo>

engine::CBlender::CBlender(CGraphicalOutput& _Gfx, bool _Blend, std::optional<std::array<float, 4>> _Factors)
	: m_HasBlend(_Blend)
{
	if (_Factors)
	{
		m_Factors = *_Factors;
	}

	HRESULT hr;
	D3D11_BLEND_DESC db = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	D3D11_RENDER_TARGET_BLEND_DESC& brt = db.RenderTarget[0];
	if (_Blend)
	{
		if (_Factors)
		{
			brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
			brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
		}
		else
		{
			brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		}
	}

	_ENGINE_MAYTHROW_GRAPHICS(GetDevice(_Gfx)->CreateBlendState(&db, &m_BlendState));
}

void engine::CBlender::SetFactor(int _Which, float _Factor) noexcept
{
	if (_Which & __hl2_BLEND_RED)
	{
		m_Factors[0] = _Which;
	}

	if (_Which & __hl2_BLEND_GREEN)
	{
		m_Factors[1] = _Which;
	}

	if (_Which & __hl2_BLEND_BLUE)
	{
		m_Factors[2] = _Which;
	}

	if (_Which & __hl2_BLEND_ALPHA)
	{
		m_Factors[3] = _Which;
	}
}

std::array<float, 4> engine::CBlender::GetFactors() const noexcept
{
	return m_Factors;
}

void engine::CBlender::Bind(CGraphicalOutput& _Gfx) noexcept
{
	GetContext(_Gfx)->OMSetBlendState(m_BlendState.Get(), m_Factors.data(), 0xFFFFFFFFu);
}

std::string engine::CBlender::Discriminate(bool _Blend, std::optional<std::array<float, 4>> _Factors)
{
#define _s(I) std::to_string((*_Factors)[I])
	using namespace std::string_literals;
	return std::string(typeid(engine::CBlender).name()) + "?"s + (_Factors ? _s(0) + _s(1) + _s(2) + _s(3) : "XAA");
#undef _s
}

std::string engine::CBlender::GenerateDiscriminator() const noexcept
{
	return this->Discriminate(m_HasBlend, { m_Factors });
}
