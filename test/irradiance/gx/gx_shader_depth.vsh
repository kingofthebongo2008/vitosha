cbuffer per_object
{
	float4x4 m_pvw;
}

struct vs_input
{
	half4	position	: POSITION;
};

struct vs_output
{
    float4	position	: SV_POSITION; 
};

vs_output main( in  vs_input input)
{
	vs_output output;
	output.position = mul ( m_pvw, float4(input.position) )  ;	
	return output;
}



