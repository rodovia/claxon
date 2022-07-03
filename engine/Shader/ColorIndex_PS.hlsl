cbuffer CB2
{
	float4 facecolors[8];
};

float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
	return facecolors[(tid / 2) % 8];
}