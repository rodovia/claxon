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

#include "PhongBase.hlsli"

cbuffer CObject : register(b10)
{
    bool EnableSpecular;
    bool HasGloss;
    float SpecularPowerConst;
    // ^^^^^ Unused in this file, only for compat with PhongSpecular_PS.hlsl
    
    float SpecularIntensity;
    float SpecularPower;
};

Texture2D g_Texture;
SamplerState g_Sampler;

float4 main(float3 _WorldPos : Position, float3 _Norm : Normal, float2 _TexCoord : Texcoord, float4 _Pos : SV_Position) : SV_TARGET
{
    _Norm = normalize(_Norm);
    
    // fragment to light vector data
    const LightVectorData ld = CalcLightVD(LightPos, _WorldPos);
    const float att = Attenuate(ld.DistanceToLight, AttConst, AttLin, AttQuad);

    const float3 diffuse = Diffuse(_Norm, ld.DirectionToLight, att, DiffColor, DiffIntensity);
    const float3 specular = Speculate(_Norm, ld.VectorToLight, att, _WorldPos, SpecularPower, DiffColor,
                                        DiffIntensity, SpecularIntensity);
    return float4(saturate((diffuse + Ambient) * g_Texture.Sample(g_Sampler, _TexCoord).rgb + specular), 1.0f);
}
