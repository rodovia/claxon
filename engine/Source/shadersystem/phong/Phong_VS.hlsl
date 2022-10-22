cbuffer ConstBuffer
{
    matrix ModelView;
    matrix ModelViewProj;
};

struct VSOut
{
    float3 ViewPos : Position;
    float3 Normal : Normal;
    float2 TexCoord : Texcoord;
    float4 Position : SV_POSITION;
};

VSOut main(float3 _Pos : Position, float3 _Normal : Normal, float2 _Tc : Texcoord)
{
    VSOut vso;
    vso.ViewPos = (float3) mul(float4(_Pos, 1.0f), ModelView);
    vso.Normal = mul(_Normal, (float3x3) ModelView);
    vso.TexCoord = _Tc;
    vso.Position = mul(float4(_Pos, 1.0f), ModelViewProj);
    return vso;
}