#include <engine_tier1/Surface.h>
#include <engine_tier0/_Math.h>
#include <tier0lib/String0.h>
#include <DirectXMath.h>
#include <string>
#include "ColorTransform.h"

static DirectX::XMVECTOR ColorToVector(engine::CColor _C);
static engine::CColor VectorToColor(DirectX::FXMVECTOR _V);

void RotateAxis180(const std::string& _PathFilei, const std::string& _PathFileo)
{
	const DirectX::XMMATRIX rot = DirectX::XMMatrixRotationX(PI_f);
	engine::CSurface surf = engine::CSurface::FromFile(tier0::ConvertToWideString(_PathFilei));

	const unsigned int npix = surf.GetWidth() * surf.GetHeight();
	engine::CColor* pb = surf.GetBufferPointer();
	const engine::CColor* pend = surf.GetBufferPointerConst() + npix;

	for (engine::CColor* pcur = pb; pcur < pend; pcur++)
	{
		DirectX::XMVECTOR ctv = ColorToVector(*pcur);
		ctv = DirectX::XMVector3Transform(ctv, rot);
		*pcur = VectorToColor(ctv);
	}

	surf.WriteToFile(tier0::ConvertToWideString(_PathFileo));
}

static DirectX::XMVECTOR ColorToVector(engine::CColor _C)
{
	DirectX::XMVECTOR vec = DirectX::XMVectorSet((float) _C.GetR(), (float) _C.GetG(), (float)_C.GetB(), (float)0.0f);
	const DirectX::XMVECTOR all255 = DirectX::XMVectorReplicate(2.0f / 255);
	vec = DirectX::XMVectorMultiply(vec, all255);

	const DirectX::XMVECTOR all1 = DirectX::XMVectorReplicate(1.0f);
	vec = DirectX::XMVectorSubtract(vec, all1);
	return vec;
}

static engine::CColor VectorToColor(DirectX::FXMVECTOR _V)
{
	const DirectX::XMVECTOR all1 = DirectX::XMVectorReplicate(1.0f);
	DirectX::XMVECTOR nouth = DirectX::XMVectorAdd(_V, all1);
	DirectX::FXMVECTOR all255 = DirectX::XMVectorReplicate(255.0f / 2.0f);
	nouth = DirectX::XMVectorMultiply(nouth, all255);
	DirectX::XMFLOAT3 floats;

	DirectX::XMStoreFloat3(&floats, nouth);
	return
	{
		(unsigned char)round(floats.x),
		(unsigned char)round(floats.y),
		(unsigned char)round(floats.z)
	};
}

