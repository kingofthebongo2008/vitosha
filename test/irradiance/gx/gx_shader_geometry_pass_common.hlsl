#ifndef __GX_SHADER_GEOMETRY_PASS_COMMON_H__

#define SLOT_PER_PASS b0
#define SLOT_PER_DRAW_CALL b1

cbuffer per_pass : register(SLOT_PER_PASS)
{
	row_major float4x4 m_v;	//view matrix
	row_major float4x4 m_p;	//projection matrix

	float4			   m_reverse_projection;	//from perspective space to view space params
}

float3 unproject_vertex(float3 v_ps)
{
	return float3(0.0,0.0,0.0);
}

float4 project_vertex( float4 v_os, float4x4 world, float4x4 view, float4x4 perspective)
{
	float4x4 wvp = mul ( world, mul ( view, perspective ) );
	return  mul ( v_os, wvp );
}

#endif