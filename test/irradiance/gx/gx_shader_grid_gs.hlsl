#include "gx_shader_geometry_pass_common.hlsl"

cbuffer per_object : register(SLOT_PER_DRAW_CALL)
{
	row_major float4x4 m_w; 
	row_major float4x4 m_normal_transform; 
}

struct gs_input
{
	half4	position_os	: sv_position; 
	//float3  normal_ws	: normal;
};

struct gs_output
{
    float4  position_ps	    : sv_position;
	float3  normal_ws       : normal;
};

[maxvertexcount(6)]
void main( point gs_input input[1], inout TriangleStream<gs_output> grid_stream )
{
    const float3 normal_ws = mul ( float4(0.0f, 1.0f, 0.0f, 0.0f), m_normal_transform ).xyz;
	
    gs_output o;

    o.position_ps	= project_vertex( float4(5.0f, 0.0f, 5.0f, 1.0f), m_w, m_v, m_p );
    o.normal_ws     = normal_ws; 
    
	grid_stream.Append(o);


	o.position_ps	= project_vertex(float4(-5.0f, 0.0f, -5.0f, 1.0f), m_w, m_v, m_p);
    grid_stream.Append(o);

	o.position_ps	= project_vertex(float4(-5.0f, 0.0f, 5.0f, 1.0f), m_w, m_v, m_p);
    grid_stream.Append(o);


    grid_stream.RestartStrip();

	o.position_ps	= project_vertex(float4(-5.0f, 0.0f, -5.0f, 1.0f), m_w, m_v, m_p);
    grid_stream.Append(o);

	o.position_ps	= project_vertex(float4(5.0f, 0.0f, 5.0f, 1.0f), m_w, m_v, m_p);
    grid_stream.Append(o);

	o.position_ps	= project_vertex( float4(5.0f, 0.0f, -5.0f, 1.0f), m_w, m_v, m_p);
    grid_stream.Append(o);

    grid_stream.RestartStrip();
}

