cbuffer per_object
{
	row_major float4x4 m_wvp; 
}

struct vs_input
{
	half4	position_ps	: POSITION;
	half2	uv			: TexCoord;
};

struct vs_output
{
    float4	position_ps	: SV_POSITION; 
	float2	uv			: TexCoord; 
};

vs_output main( in  vs_input input)
{
	vs_output output;
	output.position_ps = mul ( float4(input.position_ps), m_wvp )  ;	
	output.uv = float2(input.uv);
	return output;
}

