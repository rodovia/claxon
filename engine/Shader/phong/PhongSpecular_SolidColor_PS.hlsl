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
	float4 MaterialColor;
	bool EnableSpecular;
	bool HasGloss;
	float SpecularPowerConst;
	float3 SpecularColor;
	float SpecularPower;
};

Texture2D g_Specular : register(t1);
SamplerState g_Sampler;

float4 main(float3 _WorldPos : Position, float3 _Norm : Normal, 
			float2 _TexCoord : Texcoord, float4 _PPos : SV_Position
) : SV_TARGET
{
    const LightVectorData ld = CalcLightVD(LightPos, _WorldPos);
	const float att = Attenuate(ld.DistanceToLight, AttConst, AttLin, AttQuad);
    const float3 diffuse = Diffuse(_Norm, ld.DirectionToLight, att, DiffColor, DiffIntensity);
    
	const float4 specularSample = g_Specular.Sample(g_Sampler, _TexCoord);
	const float3 specularReflectionColor = specularSample.rgb;
	float specularPower;
	if (HasGloss)
	{
		specularPower = pow(2.0f, specularSample.a * 13.0f);
	}
	else
	{
		specularPower = SpecularPowerConst;
	}
    const float4 specular = float4(SpeculateSL(_Norm, ld.VectorToLight, att, _WorldPos, specularPower, DiffColor, DiffIntensity, SpecularColor), 0.0f);
    return saturate(float4(diffuse + Ambient, 1.0f) * MaterialColor + specular * specularSample);
}
