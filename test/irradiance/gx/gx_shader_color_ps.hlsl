struct vs_output
{
    float4	position_ps	: sv_position; 
};

cbuffer per_object
{
	float4 m_color;
}

float3  main( in  vs_output input) : sv_target
{
	return m_color.xyz;
}

