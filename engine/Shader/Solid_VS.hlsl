cbuffer Cuffer
{
    matrix Model;
    matrix ModelViewProj;
};

float4 main(float3 _Pos : Position): SV_Position
{
    return mul(float4(_Pos, 1.0f), ModelViewProj);
}
