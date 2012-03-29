cbuffer per_object
{
	row_major float4x4 m_wvp; 
}

struct vs_input
{
	half4	position_os	: POSITION;
	half4	normal_os	: Normal0;
	half2	uv			: TexCoord0;
};

struct vs_output
{
    float4	position_ps	: SV_POSITION; 
	float3  normal_os	: Normal0;
	float2	uv			: TexCoord0; 
};

vs_output main( in  vs_input input)
{
	vs_output output;

	output.position_ps	=	mul ( float4(input.position_os), m_wvp )  ;	
	output.uv			=	float2(input.uv);
	output.normal_os	=	float3(input.normal_os.xyz);

	return output;
}

