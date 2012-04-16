struct vs_output
{
    float4	position_ps	: sv_position; 
	float2	uv			: texcoord; 
};

float4 main( in  vs_output input) : sv_target
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}

