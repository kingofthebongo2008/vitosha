cbuffer per_object
{
	row_major float4x4 m_wvp;
}

struct vs_input
{
	half4	position_os	: POSITION;
};

struct vs_output
{
    float4	position_ps	: SV_POSITION; 
};

vs_output main( in  vs_input input)
{
	vs_output output;
	output.position_ps = mul ( float4(input.position_os), m_wvp )  ;	
	return output;
}



