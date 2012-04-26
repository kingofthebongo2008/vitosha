cbuffer per_draw_call : register(c0)
{
    row_major float4x4 m_inverse_projection;
    row_major float4x4 m_view; 
    float4             m_light_position_ws[8];
    float4             m_light_color[8];
    uint               m_light_count;
};


struct vs_output
{
    float4  position_ps	        : sv_position;
    float2  uv                  : texcoord;
};


Texture2D		normal_gbuffer  : register(t0);
Texture2D		diffuse_gbuffer : register(t1);
Texture2D		specular_gloss_gbuffer : register(t2);
Texture2D       depth_buffer : register(t3);;

SamplerState	default_sampler;


float3  convert_to_view_space ( float x, float y, float depth_buffer_z)
{
    //translate from perspective space to view space with inverse matrix

    float4 ps = float4(x, y, depth_buffer_z, 1.0f);
    float4 v = mul( m_inverse_projection, ps );
    float4 result = ps / ps.w;
    return result.xyz;
}

float3 convert_to_view_space ( float2 texture_coord, float depth_buffer_z)
{
    //texture_coord is in[0;1] should go in [-1 ; 1]
    float2 scale     = float2(2.0f, -2.0f);
    float2 translate = float2(-1.0f, -1.0f);

    float2 result = texture_coord * scale + translate;

    return convert_to_view_space( result.x, result.y, depth_buffer_z );
}

float4 main( in  vs_output input) : sv_target
{
	float3 f_1 = diffuse_gbuffer.Sample(default_sampler, input.uv).xyz;
    float3 f_2 = normal_gbuffer.Sample(default_sampler, input.uv).xyz;
    float4 f_3 = specular_gloss_gbuffer.Sample(default_sampler, input.uv);

    float depth_buffer_z = depth_buffer.Sample(default_sampler, input.uv).x;

    if ( depth_buffer_z == 1.0f ) discard;

    float3 surface_sample_position_vs = convert_to_view_space ( input.uv, depth_buffer_z );

    return float4(surface_sample_position_vs.z, surface_sample_position_vs.z, surface_sample_position_vs.z, 1.0f);

    //return float4(f_2, 1.0f);
    //return float4(f_3);
}


