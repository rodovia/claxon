cbuffer CBuf
{
	float4 Color;
};


float4 main(float3 _Pos : Position) : SV_TARGET
{
	return Color;
}
