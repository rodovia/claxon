Texture2D textureObj;

SamplerState sam;

float4 main(float2 _Tc : TexCoord, float4 _Pos : Position) : SV_TARGET
{
    float4 pixelColor = textureObj.Sample(sam, _Tc);
	return pixelColor;
}