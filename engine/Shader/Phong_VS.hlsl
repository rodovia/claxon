cbuffer ConstBuffer
{
    matrix Model;
    matrix ModelViewProj;
};

struct VSOut
{
    float3 WorldPos : Position;
    float3 Normal : Normal;
    float4 Pos : SV_Position;
};

float4 main(float3 _Pos : Position, float3 _Normal : Normal) : SV_POSITION
{
    VSOut vso;
    vso.WorldPos = (float3)mul(float4(_Pos, 1.0f), Model);
    vso.Normal = mul(_Normal, (float3x3) Model);
    vso.Pos = mul(float4(_Pos, 1.0f), ModelViewProj);
    return vso;
}