cbuffer per_object
{
	row_major float4x4 m_wvp; 
	row_major float4x4 m_normal_transform; 
}

struct vs_input
{
	half4	position_os	: position;
	half4	normal_os	: normal;
};

struct vs_output
{
    float4	position_ps	: sv_position; 
	float3  normal_ws	: normal;
};

vs_output main( in  vs_input input)
{
	vs_output output;

	//get rid of translation (assume no scaling)
	float4x4 normal_transform = m_normal_transform;
	normal_transform[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	output.position_ps	=	mul ( float4(input.position_os), m_wvp )  ;	
	output.normal_ws	=	mul ( float4(input.normal_os.xyz, 1.0f), normal_transform ).xyz;
    
	return output;
}

