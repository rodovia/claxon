/* 
 Difere do Texture_VS tal que recebe 3 argumentos (os 2 + normal) 
 e ignora o normal. Existe por conta do CPrim_Plane. Tem de ser utilizado em conjunto ao
 Texture_PS
*/

cbuffer Trnsf
{
    matrix Model;
    matrix ModelViewProj;
};

struct VSOut
{
    float2 Tex : TexCoord;
    float4 Coord : SV_Position;
};

VSOut main(float3 _Pos : Position, float2 _Tc : TexCoord)
{
    VSOut vso;
    vso.Tex = _Tc;
    vso.Coord = mul(float4(_Pos, 1.0f), ModelViewProj);
    return vso;
}