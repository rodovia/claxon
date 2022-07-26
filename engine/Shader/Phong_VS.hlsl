cbuffer ConstBuffer
{
    matrix ModelView;
    matrix ModelViewProj;
};

struct VSOut
{
    float3 WorldPos : Position;
    float3 Normal : Normal;
    float4 Pos : SV_Position;
    float2 TexCoord : Texcoord;
};

VSOut main(float3 _Pos : Position, float3 _Normal : Normal, float2 _Tc : Texcoord)
{
    VSOut vso;
    vso.WorldPos = (float3)mul(float4(_Pos, 1.0f), ModelView);
    vso.Normal = mul(_Normal, (float3x3) ModelView);
    vso.Pos = mul(float4(_Pos, 1.0f), ModelViewProj);
    vso.TexCoord = _Tc;
    return vso;
}