struct vs_output
{
    float4	position_ps	: SV_POSITION; 
};

cbuffer per_object
{
	float4 m_color;
}

float3  main( in  vs_output input) : SV_Target
{
	return m_color.xyz;
}

