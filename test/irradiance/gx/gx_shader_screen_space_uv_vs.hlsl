#include "gx_shader_geometry_pass_common.hlsl"

cbuffer per_object
{
	row_major float4x4 m_wvp; 
}

struct vs_input
{
	half4	position_ps	: position;
	half2	uv			: texcoord;
};

struct vs_output
{
    float4	position_ps	    : sv_position;
	float2	uv			    : texcoord; 
    float2	position2_ps	: position_ps; 
};

vs_output main( in  vs_input input)
{
	vs_output output;
	output.position_ps = mul ( float4(input.position_ps), m_wvp )  ;	
	output.uv = float2(input.uv);
    output.position2_ps = output.position_ps.xy;
	return output;
}

