Texture2D g_Texture : register(t0);
SamplerState g_Sampler;

float4 main(float3 _Pos : Position, float3 _Ignored1 : Normal, float3 _Ignored2 : Tangent,
            float3 _Ignored3 : Bitangent, float2 _Tc : Texcoord) : SV_TARGET
{
    return g_Texture.Sample(g_Sampler, _Tc);
}