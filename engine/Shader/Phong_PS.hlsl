cbuffer CLight
{
    float3 LightPos;
};

static const float3 g_MaterialColor = { 0.7f, 0.7f, 0.9f };
static const float3 g_Ambient = { 0.15f, 0.15f, 0.15f };
static const float3 g_DiffColor = { 1.0f, 1.0f, 1.0f };
static const float g_DiffIntensity = 1.0f;
static const float g_AttConst = 1.0f;
static const float g_AttLin = 0.045f;
static const float g_AttQuad = 0.0075f;

float4 main(float3 _WorldPos: Position, float3 _Norm: Normal): SV_TARGET
{
    // fragment to light vector data
    const float3 vtol = LightPos - _WorldPos;
    const float distToL = length(vtol);
    const float3 dirToL = vtol / distToL;
    
    // diffuse attenuation
    const float att = 1.0f / g_AttConst + g_AttLin * distToL + g_AttQuad * pow(distToL, 2);
    const float3 diff = g_DiffColor * g_DiffIntensity * att * max(0.0f, dot(dirToL, _Norm));
    
    return float4(saturate(diff + g_Ambient), 1.0f);
}
