struct LightVectorData
{
    float3 VectorToLight;
    float DistanceToLight;
    float3 DirectionToLight;
};

LightVectorData CalcLightVD(const float3 _LightPos, 
                            const float3 _WorldPos : Position)
{
    LightVectorData od;
    od.VectorToLight = _LightPos - _WorldPos;
    od.DistanceToLight = length(od.VectorToLight);
    od.DirectionToLight = od.VectorToLight / od.DistanceToLight;
    return od;
}

float3 CalcMapNormalViewSpace(const float3 _Tan : Tangent,
                              const float3 _Bit : Bitangent,
                              const float3 _Norm : Normal,
						      const float2 _TexCoord : TexCoord,
                              const Texture2D _NormalMap,
                              const SamplerState _SState)
{
    const float3x3 tanview = float3x3(_Tan, _Bit, _Norm);
    float3 tangentNormal;
    const float3 normalSample = _NormalMap.Sample(_SState, _TexCoord).xyz;
    tangentNormal = normalSample * 2.0f - 1.0f;
    tangentNormal.y = -_Norm.y;
        
    return normalize(mul(tangentNormal, tanview));
}

float3 Diffuse(const float3 _Norm : Normal,
               const float3 _DirToL,
               const float _Att,
               const float3 _DiffColor,
               const float _DiffIntensity)
{
    return _DiffColor * _DiffIntensity * _Att * max(0.0f, dot(_DirToL, _Norm));
}

float3 Speculate(const in float3 _Norm : Normal,
                const in float3 _VToL,
                const float _Att,
                const float3 _WorldPos : Position,
                const float _SpecularPower, 
                const float3 _DiffColor,
                const float _DiffIntensity,
                const float _SpecularIntensity)
{
    const float3 w = _Norm * dot(_VToL, _Norm);
    const float3 r = w * 2.0f - _VToL;
    const float3 vcToFrag = normalize(_WorldPos);
    return _Att * (_DiffColor * _DiffIntensity) * _SpecularIntensity *
            pow(max(0.0f, dot(normalize(-r), vcToFrag)), _SpecularPower);
}

// SL - Solid Color
float3 SpeculateSL(const in float3 _Norm : Normal,
                const in float3 _VToL,
                const float _Att,
                const float3 _WorldPos : Position,
                const float _SpecularPower,
                const float3 _DiffColor,
                const float _DiffIntensity,
                const float3 _SpecularColor)
{
    const float3 w = _Norm * dot(_VToL, _Norm);
    const float3 r = w * 2.0f - _VToL;
    const float3 vcToFrag = normalize(_WorldPos);
    return _Att * (_DiffColor * _DiffIntensity) * _SpecularColor *
            pow(max(0.0f, dot(normalize(-r), vcToFrag)), _SpecularPower);
}

float Attenuate(float _DistToL,
                float _AttConst,
                float _AttLin,
                float _AttQuad)
{
    return 1.0f / (_AttConst + _AttLin * _DistToL + _AttQuad * (_DistToL * _DistToL));
}
