/* SolidTexture_VS.hlsl -- Used when MODEL_DESCRIPTOR specified false for AffectedByLight field. */

struct VSOut
{
	float3 WorldPosition : Position;
    float3 _Ignored1 : Normal;
    float3 _Ignored2 : Tangent;
    float3 _Ignored3 : Bitangent;
	float2 TexCoord : Texcoord;
};

VSOut main(float3 _Pos : Position, float3 _Ignored1 : Normal, float3 _Ignored2 : Tangent,
            float3 _Ignored3 : Bitangent, float2 _Tc : Texcoord)
{
	VSOut vso;
	vso.WorldPosition = _Pos;
    vso._Ignored1 = _Ignored1;
    vso._Ignored2 = _Ignored2;
    vso._Ignored3 = _Ignored3;
	vso.TexCoord = _Tc;
	return vso;
}
