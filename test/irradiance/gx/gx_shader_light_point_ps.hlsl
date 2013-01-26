#include "gx_shader_geometry_pass_common.hlsl"
#include "gx_shader_lighting.hlsl"

cbuffer per_draw_call : register(SLOT_PER_DRAW_CALL)
{
    row_major float4x4 m_inverse_projection;
    row_major float4x4 m_view; 
    float4             m_light_position_vs[8];
    float4             m_light_color_range[8];
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

SamplerState	default_sampler : register(s0);

float3  convert_to_view_space ( float x, float y, float depth_buffer_z)
{
	return reconstruct_xyz_vs(  float2 (x,y), depth_buffer_z);
}

float3 convert_to_view_space ( float2 texture_coord, float depth_buffer_z)
{
    //texture_coord is in[0;1] should go in [-1 ; 1]
    float2 scale     = float2(-2.0f, -2.0f);
    float2 translate = float2( 1.0f, 1.0f);

    float2 result = texture_coord * scale + translate;
    return convert_to_view_space( result.x, result.y, depth_buffer_z );
}

float3 main( in  vs_output input) : sv_target
{
    float2 uv					    = input.uv;

	float3 kd					    = diffuse_gbuffer.Sample(default_sampler, uv).xyz;
	float4 n_vs_gloss				= normal_gbuffer.Sample( default_sampler, uv );
    float3 n_vs						= normalize( n_vs_gloss.xyz );
    float4 ks_gloss			        = specular_gloss_gbuffer.Sample(default_sampler, uv);

    float depth_buffer_z			= depth_buffer.Sample(default_sampler, uv).x;

	//todo: fix with far plane to be at infinity
    if ( depth_buffer_z == 1.0f ) discard;

    float3 surface_sample_position_vs = convert_to_view_space ( input.uv, depth_buffer_z );

    float3 v						= -surface_sample_position_vs;

	float3 specular_color			= decode_specular_color( ks_gloss );
    float  specular_power			= decode_specular_power( n_vs_gloss );

    float3 radiance					=  float3(0.0f,0.0f,0.0f);

    for (uint i = 0 ; i < m_light_count;++i)
    {
		const float3 light_color			= decode_light_power( m_light_color_range[i] ) ;
		const float3 light_distance_vs		= m_light_position_vs[i].xyz - surface_sample_position_vs;
		const float	 distance_2				= dot ( light_distance_vs, light_distance_vs );
	    const float3 light_vs				= light_distance_vs / sqrt( distance_2 ); 
		const float3 normal_vs				= n_vs;

		const float  range					= decode_light_range( m_light_color_range[i] );
		const float  range_2				= range * range;

		if ( distance_2 < range_2 )
		{
			const float	 attenuation		= light_attenuation_2( distance_2, range_2, 16.0f);

			shaded_surface surface			= gotanda(kd, specular_color, specular_power, light_vs , normal_vs, normalize(v) );
			radiance						= radiance +  attenuation * light_color * ( surface.m_kd  + surface.m_ks) ;
		}
    }

    return radiance;
}


