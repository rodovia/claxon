#pragma once

#include <engine_tier0/BaseBind.h>
#include <engine_tier0/ComPtr.h>
#include <array>
#include <string>
#include <optional>

#define __hl2_BLEND_RED   0x11000000
#define __hl2_BLEND_GREEN 0x00220000
#define __hl2_BLEND_BLUE  0x00003300
#define __hl2_BLEND_ALPHA 0x00000044
// Helper.
#define __hl2_BLEND_ALL   0x11223344

namespace engine
{

class CBlender : public CBase_Bind
{
public:
	CBlender(CGraphicalOutput& _Gfx, bool _Blend, std::optional<std::array<float, 4>> _Factors);
	void Bind(CGraphicalOutput& _Gfx) noexcept override;
	static std::string Discriminate(bool _Blend, std::optional<std::array<float, 4>>);
	std::string GenerateDiscriminator() const noexcept;

	std::array<float, 4> GetFactors() const noexcept;
	void SetFactor(int _Which, float _Factor) noexcept;

private:
	CUtl_ComPtr<ID3D11BlendState> m_BlendState;
	bool m_HasBlend;
	std::array<float, 4> m_Factors;
};

}