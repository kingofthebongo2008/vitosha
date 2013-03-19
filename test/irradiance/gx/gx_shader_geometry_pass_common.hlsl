#ifndef __GX_SHADER_GEOMETRY_PASS_COMMON_H__
#define __GX_SHADER_GEOMETRY_PASS_COMMON_H__

#define SLOT_PER_PASS b0
#define SLOT_PER_DRAW_CALL b1

cbuffer per_pass : register(SLOT_PER_PASS)
{
    row_major float4x4 m_v;                     //view matrix
    row_major float4x4 m_p;                     //projection matrix
    float4             m_reverse_projection;    //from perspective space to view space params. holds rational transform for reconstruction of depth
}

float4 project_vertex( float4 v_os, float4x4 world, float4x4 view, float4x4 perspective)
{
    float4x4 wvp = mul ( world, mul ( view, perspective ) );
    return  mul ( v_os, wvp );
}

float reconstruct_z_vs( float depth_buffer_z, float4 rational_transform)
{
    float2 ac = float2(rational_transform.x, rational_transform.z);
    float2 bd = float2(rational_transform.y, rational_transform.w);

    float  z = depth_buffer_z;

    float2  z_1 = ac * z + bd;
    return  z_1.x / z_1.y;
}

float reconstruct_z_vs( float z_ps)
{
    return reconstruct_z_vs(z_ps, m_reverse_projection);
}

float2 reconstruct_xy_vs( float2 xy_ps, float z_vs, float2 transform)
{
    return (xy_ps * z_vs) / transform;
}

float2 reconstruct_xy_vs( float2 xy_ps, float z_vs)
{
    float2 transform = float2(m_p._11, m_p._22);
    return reconstruct_xy_vs(xy_ps, z_vs, transform);
}

float3 reconstruct_xyz_vs(float2 xy_ps, float z_ps)
{
    float z_vs = reconstruct_z_vs(z_ps);
    float2 xy_vs = reconstruct_xy_vs(xy_ps, z_vs);
    return float3(xy_vs, z_vs);
}

#endif