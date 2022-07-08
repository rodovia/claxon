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

cbuffer CObject : register(b1)
{
    float3 MaterialColor;
};

float4 main(float3 _WorldPos: Position, float3 _Norm: Normal): SV_TARGET
{
    // fragment to light vector data
    const float3 vtol = LightPos - _WorldPos;
    const float distToL = length(vtol);
    const float3 dirToL = vtol / distToL;
    
    // diffuse attenuation
    const float att = 1.0f / AttConst + AttLin * distToL + AttQuad * pow(distToL, 2);
    const float3 diff = DiffColor * DiffIntensity * att * max(0.0f, dot(dirToL, _Norm));
    
    return float4(saturate((diff + Ambient) * MaterialColor), 1.0f);
}
