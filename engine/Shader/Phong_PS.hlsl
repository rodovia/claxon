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
    float3 MaterialColor;
    float SpecularIntensity;
    float SpecularPower;
};

float3 CalculateSpecularIntensity(float3 _VecToLight, float _Att, float3 _WorldPos: Position, float3 _Normal : Normal)
{
    const float3 w = _Normal * dot(_VecToLight, _Normal);
    const float3 r = w * 2.0f - _VecToLight;
    return _Att * (DiffColor * DiffIntensity) * SpecularIntensity * 
            pow(max(0.0f, dot(normalize(-r), normalize(_WorldPos))), SpecularPower);
}

float4 main(float3 _WorldPos: Position, float3 _Norm: Normal): SV_TARGET
{
    // fragment to light vector data
    const float3 vtol = LightPos - _WorldPos;
    const float distToL = length(vtol);
    const float3 dirToL = vtol / distToL;
    
    // diffuse attenuation
    const float att = 1.0f / AttConst + AttLin * distToL + AttQuad * pow(distToL, 2);
    const float3 diff = DiffColor * DiffIntensity * att * max(0.0f, dot(dirToL, _Norm));
    
    return float4(saturate((diff + Ambient + CalculateSpecularIntensity(vtol, att, _WorldPos, _Norm)) * MaterialColor), 1.0f);
}
