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
	float4 MaterialColor;
	bool EnableSpecular;
	bool HasGloss;
	float SpecularPowerConst;
	float SpecularIntensity;
	float SpecularPower;
};

Texture2D g_Specular : register(t1);
SamplerState g_Sampler;

float4 main(float3 _WorldPos : Position, float3 _Norm : Normal, 
			float2 _TexCoord : Texcoord, float4 _PPos : SV_Position
) : SV_TARGET
{
	const float3 vToL = LightPos - _WorldPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
    
	const float att = 1.0f / (AttConst + AttLin * distToL + AttQuad * (distToL * distToL));
	const float3 diffuse = DiffColor * DiffIntensity * att * max(0.0f, dot(dirToL, _Norm));

	const float3 w = _Norm * dot(vToL, _Norm);
	const float3 r = w * 2.0f - vToL;
    
	const float4 specularSample = g_Specular.Sample(g_Sampler, _TexCoord);
	const float3 specularReflectionColor = specularSample.rgb;
	float specularPower;
	if (HasGloss)
	{
		specularPower = pow(2.0f, specularSample.a * 13.0f);
	}
	else
	{
		specularPower = SpecularPowerConst;
	}
	const float3 specular = att * (DiffColor * DiffIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(_WorldPos))), specularPower);

	return float4(saturate((diffuse + Ambient) * MaterialColor.rgb + specular * specularReflectionColor), 1.0f);
}
