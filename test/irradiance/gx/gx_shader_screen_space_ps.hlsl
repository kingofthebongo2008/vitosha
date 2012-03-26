struct vs_output
{
    float4	position_ps	: SV_POSITION; 
	float2	uv			: TexCoord; 
};

float4 main( in  vs_output input) : SV_Target
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}

