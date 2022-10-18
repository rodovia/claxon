cbuffer CBuf
{
	matrix transform;
};

struct VSInput
{
	float3 pos : Position;
	float2 tex : TexCoord;
};

struct VSOut
{
	float2 tex : TexCoord;
	float4 pos : SV_Position;
};

VSOut main(VSInput input)
{
	VSOut vso;
	vso.tex = input.tex;
	vso.pos = mul(float4(input.pos, 1.0f), transform);
	return vso;
}