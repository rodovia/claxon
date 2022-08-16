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
    float SpecularIntensity;
    float SpecularPower;
    bool EnableNormalMap;
    float _padding[1];
};

cbuffer CTransform : register(b1)
{
    matrix ModelView;
    matrix ModelViewProj;
};

#include "PhongBase.hlsli"

// Slot 0 - Diff. Texture
// Slot 1 - Specular Texture
// Slot 2 - Normal map Texture
Texture2D g_Texture;
Texture2D g_NormalMap : register(t2);
SamplerState g_Sampler;

float4 main(float3 _WorldPos : Position, float3 _Norm : Normal, 
        float3 _Tan : Tangent, float3 _Bit : Bitangent, float2 _TexCoord : Texcoord) : SV_TARGET
{
    if (EnableNormalMap)
    {
        const float3 normalSample = g_NormalMap.Sample(g_Sampler, _TexCoord).xyz;
        const float3 objNorm = normalSample * 2.0f - 1.0f;
        _Norm = normalize(mul(objNorm, (float3x3) ModelView));
    }
	
	// fragment to light vector data
    const LightVectorData ld = CalcLightVD(LightPos, _WorldPos);
    const float att = Attenuate(ld.DistanceToLight, AttConst, AttLin, AttQuad);

    const float3 diffuse = Diffuse(_Norm, ld.DirectionToLight, att, DiffColor, DiffIntensity);
    const float3 specular = Speculate(_Norm, ld.VectorToLight, att, _WorldPos, SpecularPower,
                                        DiffColor, DiffIntensity, SpecularIntensity);

    return float4(saturate((diffuse + Ambient) * g_Texture.Sample(g_Sampler, _TexCoord).rgb + specular), 1.0f);
}