struct PSIn
{
	float2 tex: TexCoord;
	float4 pos : SV_POSITION;
};

Texture2D textureObj;

SamplerState sam;

float4 main(PSIn ps) : SV_TARGET
{
	float4 pixelColor = textureObj.Sample(sam, ps.tex);
	return pixelColor;
}