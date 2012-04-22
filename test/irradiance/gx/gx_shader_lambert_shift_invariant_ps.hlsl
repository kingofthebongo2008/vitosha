#include <gx/gx_shader_brdf_hlsl.h>

struct vs_output
{
    float4	position_ps	: sv_position;
	float3  normal_ws	: normal;
};

cbuffer per_object
{
	float4 m_kd;
}

struct specular
{
	float3	m_ks;		// (0.05 is for plastic for example, generally > 0.5 for metals ) . Dielectric materials do not have specular color, just metals
	float	m_gloss;	// gloss ( specular power = pow( l_max , m_gloss )  ( l_max is the maximum specular power in the game
};

struct ps_output
{
	float3	 m_kd			: sv_target0;
    float4   m_normal_depth : sv_target1;
	float4	 m_ks_gloss		: sv_target2;
};

ps_output main( in  vs_output input)
{
	ps_output result;

    result.m_kd = m_kd;
    result.m_normal_depth = float4(input.normal_ws, 0.5f) ;
	result.m_ks_gloss = float4(0.0f, 0.0f, 0.0f, 0.0f);
    result.m_ks_gloss = float4(1.0f, 0.0f, 0.0f, 0.0f);

	return result;
}
