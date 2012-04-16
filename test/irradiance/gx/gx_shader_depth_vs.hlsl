cbuffer per_object
{
	row_major float4x4 m_wvp;
}

struct vs_input
{
	half4	position_os	: position;
};

struct vs_output
{
    float4	position_ps	: sv_position; 
};

vs_output main( in  vs_input input)
{
	vs_output output;
	output.position_ps = mul ( float4(input.position_os), m_wvp )  ;	
	return output;
}



