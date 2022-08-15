cbuffer CLight : register(b0)
{
	float3 LightPos;
	float3 Ambient;
	float3 DiffColor;
	float DiffIntensity;
	float AttConst;
	float AttLin;
	float AttQuad;
};

cbuffer CObject : register(b10)
{
	float SpecularPower;
    float3 SpecularColor;
    float SpecularMapWeight;
	bool EnableNormalMap;
    bool EnableSpecularMap;
    bool HasGloss;
    float SpecularIntensity;
};

// Slot 0 - Diff. Texture
// Slot 1 - Specular Texture
// Slot 2 - Normal map Texture
Texture2D g_Texture;
Texture2D g_Specular;
Texture2D g_NormalMap : register(t2);
SamplerState g_Sampler;

float4 main(float3 _WorldPos : Position, float3 _Norm : Normal, float3 _Tan : Tangent, 
				float3 _Bit : Bitangent, float2 _TexCoord : Texcoord, float4 _Pos : SV_Position) : SV_TARGET
{
	if (EnableNormalMap)
	{
        const float3x3 tanview = float3x3(
			normalize(_Tan),
			normalize(_Bit),
			normalize(_Norm)
		);
        const float3 normalSample = g_NormalMap.Sample(g_Sampler, _TexCoord).xyz;
        _Norm = normalSample * 2.0f - 1.0f;
        _Norm.y = -_Norm.y;
        //_Norm = mul(_Norm, (float3x3) ModelView);
        _Norm = mul(_Norm, tanview);
    }
	
	// fragment to light vector data
	const float3 vToL = LightPos - _WorldPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
	const float att = 1.0f / (AttConst + AttLin * distToL + AttQuad * (distToL * distToL));

    float3 specularReflectionColor;
    float specularPower = SpecularPower;
	if (EnableSpecularMap)
    {
        const float4 specularSample = g_Specular.Sample(g_Sampler, _TexCoord);
        specularReflectionColor = specularSample.rgb * SpecularMapWeight;
        if (HasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
	else
    {
        specularReflectionColor = SpecularColor;
    }
	
	const float3 diffuse = DiffColor * DiffIntensity * att * max(0.0f, dot(dirToL, _Norm));
	const float3 w = _Norm * dot(vToL, _Norm);
	const float3 r = w * 2.0f - vToL;
	const float3 specular = att * (DiffColor * DiffIntensity) * SpecularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(_WorldPos))), SpecularPower);

	return float4(saturate((diffuse + Ambient) * g_Texture.Sample(g_Sampler, _TexCoord).rgb 
					+ specular * specularReflectionColor), 1.0f);
}