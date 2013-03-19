#include "gx_shader_geometry_pass_common.hlsl"

cbuffer per_object : register(SLOT_PER_DRAW_CALL)
{
    row_major float4x4 m_w; 
}

struct vs_input
{
    half4	position_os	: position;
    half2	uv			: texcoord;
};

struct vs_output
{
    float4	position_ps	: sv_position;
    float2	uv			: texcoord; 
};

vs_output main( in vs_input input )
{
    vs_output output;

    output.position_ps = project_vertex(input.position_os, m_w, m_v, m_p);
    output.uv = float2(input.uv);

    return output;
}

