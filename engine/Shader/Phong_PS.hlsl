/*
	Phong_PS.hlsl -- Used when a model has no specular textures
*/

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
    bool EnableSpecular;
    bool HasGloss;
    float SpecularPowerConst;
    // ^^^^^ Unused in this file, only for compat with PhongSpecular_PS.hlsl
    
    float SpecularIntensity;
    float SpecularPower;
};

Texture2D g_Texture;
SamplerState g_Sampler;

// TODO: change _WorldPos to _ViewPos so it won't be misleading
// anymore.
float4 main(float3 _WorldPos : Position, float3 _Norm : Normal, float2 _TexCoord : Texcoord, float4 _Pos : SV_Position) : SV_TARGET
{
    // fragment to light vector data
    const float3 vToL = LightPos - _WorldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    const float att = 1.0f / (AttConst + AttLin * distToL + AttQuad * (distToL * distToL));

    const float3 diffuse = DiffColor * DiffIntensity * att * max(0.0f, dot(dirToL, _Norm));
    const float3 w = _Norm * dot(vToL, _Norm);
    const float3 r = w * 2.0f - vToL;
    const float3 specular = att * (DiffColor * DiffIntensity) * SpecularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(_WorldPos))), SpecularPower);

    return float4(saturate((diffuse + Ambient) * g_Texture.Sample(g_Sampler, _TexCoord).rgb + specular), 1.0f);
}
