cbuffer CBuf
{
    matrix ModelView;
    matrix ModelViewProj;
};

struct VSOut
{
    float3 WorldPos : Position;
    float3 Normal : Normal;
    float3 Color : Color;
    float4 Position : SV_Position;
};

VSOut main(float3 _Position : Position, float3 _Normal : Normal, float3 _Color : Color)
{
    VSOut o;
    o.WorldPos = (float3) mul(float4(_Position, 1.0f), ModelView);
    o.Normal = mul(_Normal, (float3x3) ModelViewProj);
    o.Position = mul(float4(_Position, 1.0f), ModelViewProj);
    o.Color = _Color;
	return o;
}