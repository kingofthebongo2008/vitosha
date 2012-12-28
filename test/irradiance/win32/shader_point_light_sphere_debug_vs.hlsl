#include "gx/gx_shader_geometry_pass_common.hlsl"

cbuffer per_object : register(SLOT_PER_DRAW_CALL)
{
	row_major float4x4 vp;	//view projection
}

struct vs_input
{
	half4				position_os				: position;
	half4				normal_os				: normal;
	half2				uv						: texcoord;
	row_major float4x4	sphere_world			: sphere_transform;
	float4				sphere_surface_color	: sphere_color;
};

struct vs_output
{
    float4	position_ps			 : sv_position;
	float4  sphere_surface_color : sphere_color;
	float3	normal_ws			 : normal;
};

vs_output main( in vs_input input)
{
	//prepare matrix for postion transformation
	float4x4  wvp = mul ( input.sphere_world, vp) ;

	//get rid of translation (assume no scaling)
	float4x4  sphere_world_normal = input.sphere_world;
	sphere_world_normal[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float4	  normal_ws = mul ( float4(input.normal_os), sphere_world_normal ) ;

	vs_output output;
	output.position_ps = mul ( float4(input.position_os), wvp )  ;	
	output.sphere_surface_color = input.sphere_surface_color;
	output.normal_ws = float3( normal_ws.xyz);
	return output;
}



