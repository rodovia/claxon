cbuffer CLight : register(b0)
{
	float3 LightPos;
	float3 Ambient;
	float3 DiffColor;
	float DiffIntensity;
	float AttConst;
	float AttLin;
	float AttQuad;
};

cbuffer CObject : register(b10)
{
	float SpecularPower;
    float3 SpecularColor;
    float SpecularMapWeight;
	bool EnableNormalMap;
    bool EnableSpecularMap;
    bool HasGloss;
    float SpecularIntensity;
};

#include "PhongBase.hlsli"

// Slot 0 - Diff. Texture
// Slot 1 - Specular Texture
// Slot 2 - Normal map Texture
Texture2D g_Texture;
Texture2D g_Specular;
Texture2D g_NormalMap : register(t2);
SamplerState g_Sampler;

float4 main(float3 _WorldPos : Position, float3 _Norm : Normal, float3 _Tan : Tangent, 
				float3 _Bit : Bitangent, float2 _TexCoord : Texcoord, float4 _Pos : SV_Position) : SV_TARGET
{
    float4 txtsmp = g_Texture.Sample(g_Sampler, _TexCoord);
    
#ifdef __hl2_want_ALPHA_CLIPPING 
    clip(txtsmp.a < 0.1f ? -1 : 1);
    if (dot(_Norm, _WorldPos) >= 0)
    {
        _Norm = -_Norm;
    }
#endif
    _Norm = normalize(_Norm);
	if (EnableNormalMap)
	{
        _Norm = CalcMapNormalViewSpace(_Tan, _Bit, _Norm, _TexCoord, g_NormalMap, g_Sampler);
    }
	
	// fragment to light vector data
	const float3 vToL = LightPos - _WorldPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
	const float att = Attenuate(distToL, AttConst, AttLin, AttQuad);

    float3 specularReflectionColor;
    float specularPower = SpecularPower;
	if (EnableSpecularMap)
    {
        const float4 specularSample = g_Specular.Sample(g_Sampler, _TexCoord);
        specularReflectionColor = specularSample.rgb * SpecularMapWeight;
        if (HasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
	else
    {
        specularReflectionColor = SpecularColor;
    }

    const float3 diffuse = Diffuse(_Norm, dirToL, att, DiffColor, DiffIntensity);
    const float3 specular = Speculate(_Norm, vToL, att, _WorldPos, specularPower, 
                                        DiffColor, DiffIntensity, SpecularIntensity);
	return float4(saturate((diffuse + Ambient) * txtsmp.rgb
					+ specular * specularReflectionColor), txtsmp.a);
}
