// =============
// Sprite.h -- A 2D texture inside a 3D world.
// =============

#include <string>

#include <engine_tier0/DLL.h>
#include <engine_tier1/Surface.h>
#include <engine_tier1/dxprim/draw/BaseDraw.h>
#include <engine_tier1/Camera.h>

namespace engine
{

struct SPRITE_DESCRIPTOR
{
	DirectX::XMFLOAT3 InitialPos;
	float Scale = 1.0f;
};

class _ENGINE_DLLEXP CSprite : public CBase_Draw
{
public:
	CSprite() = default;
	CSprite(CGraphicalOutput& _Gfx, std::wstring _Filename, SPRITE_DESCRIPTOR = {}) noexcept;

	DirectX::XMMATRIX GetTransformMatrix(const CCamera&) const noexcept override;
	void SpawnControlWindow() noexcept;
private:
	struct
	{
		// If not explicitly set to zero, Rotation will initialize in INT32_MIN
		// (for whatever reason)
		DirectX::XMFLOAT3 Rotation = {0.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 Position;
	} m_Pos;
};

}
