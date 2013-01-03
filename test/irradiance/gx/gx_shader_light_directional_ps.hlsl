#include "gx_shader_geometry_pass_common.hlsl"

cbuffer per_draw_call : register(SLOT_PER_DRAW_CALL)
{
    row_major float4x4 m_inverse_projection;
    row_major float4x4 m_view; 
    float4             m_light_direction_vs[8];
    float4             m_light_color[8];
    uint               m_light_count;
};

struct vs_output
{
    float4  position_ps	        : sv_position;
    float2  uv                  : texcoord;
};

struct shaded_surface
{
    float3 m_kd;
    float3 m_ks;
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

float3 rigid_transform_vector(float3 v, float4x4 m)
{
    return mul ( float4(v.xyz,0.0f ), m ).xyz;
}  

float3 fresnel_schlick(float3 specular_color, float3 l,float3 h)
{
    return specular_color + (1.0f - specular_color) * pow(  1.0f - saturate( dot(l, h) )  , 5);
}

float3 half_way_vector(float3 l, float3 v)
{
    return normalize( l + v );
}

float decode_specular_power( float gloss )
{
    return pow ( 2, 10 * gloss + 0.01 );
}

float decode_specular_power( float4 ks_gloss )
{
    return decode_specular_power(ks_gloss.w) ;
}

float3 decode_specular_color( float4 ks_gloss )
{
    return ks_gloss.xyz;
}

float3 decode_light_power( float4 light)
{
    return light.xyz;
}

float3 blinn_phong_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float3 n_dot_l)
{
    float3 h = half_way_vector( l, v );

    float3 fresnel = fresnel_schlick( specular_color, l, h );

	const float pi = 3.14159265f;
	float  normalization = ( (power + 2.0f) / ( 8.0f ) ); //( (power + 8.0f) / ( 8.0f * pi ) );
	//float  normalization = ( (power + 8.0f) / ( 8.0f * pi ) ); //( (power + 8.0f) / ( 8.0f * pi ) );

    float  multiplier =  normalization * pow ( saturate( dot( n, h ) ) , power ) ;

    return multiplier * fresnel * n_dot_l;
}

float3 blinn_phong_diffuse(float3 albedo, float3 l, float3 n, float3 n_dot_l )
{
	const float pi = 3.14159265f;
    return (albedo * n_dot_l) / pi;
}

shaded_surface blinn_phong(float3 albedo, float3 specular_color, float power, float3 l, float3 n, float3 v)
{
    shaded_surface result;
    float n_dot_l = dot ( n, l );

    if (n_dot_l > 0.0f)
    {
		n_dot_l = saturate(n_dot_l);
        result.m_kd = blinn_phong_diffuse ( albedo, l, n, n_dot_l);
        result.m_ks = blinn_phong_specular ( specular_color, 4.0f * power, l, n, v, n_dot_l);
    }
    else
    {
        result.m_kd = float3(0.0f, 0.0f, 0.0f);
        result.m_ks = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

float3 phong_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float3 n_dot_l)
{
    float3 h = half_way_vector( l, v );

	//float3 r = 2 * dot(v,n) * n - v; 
	float3 r = reflect( -v, n );

    float3 fresnel = fresnel_schlick( specular_color, l, h );

	const float pi = 3.14159265f;

	float  normalization = ( (power + 2.0f) / ( 2.0f * pi ) );

    float  multiplier =  normalization  * pow ( saturate( dot( r, l ) ) , power ) ;

    return multiplier * fresnel * n_dot_l;
}

shaded_surface phong(float3 albedo, float3 specular_color, float power, float3 l, float3 n, float3 v)
{
	shaded_surface result;
    float n_dot_l = dot ( n, l );

    if (n_dot_l > 0.0f)
    {
		n_dot_l = saturate(n_dot_l);
        result.m_kd = blinn_phong_diffuse ( albedo, l, n, n_dot_l);
        result.m_ks = phong_specular ( specular_color, power, l, n, v, n_dot_l);
    }
    else
    {
        result.m_kd = float3(0.0f, 0.0f, 0.0f);
        result.m_ks = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

float3 gotanda_diffuse(float3 albedo, float3 l, float3 n, float3 n_dot_l, float3 specular_color )
{
	const float pi = 3.14159265f;
	//full calculation
	
	float3 fresnel = fresnel_schlick( specular_color, n, l );
    return n_dot_l * ( albedo  *  ( 1.0f - fresnel ) ) / pi;
	

	//approximation
	//return ( albedo * (1.0f - specular_color ) ) /  pi;
}

float3 gotanda_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float3 n_dot_l)
{
    float3 h = half_way_vector( l, v );
	float  n_dot_v  = dot ( n, v );

    float3 fresnel = fresnel_schlick( specular_color, l, h );

	float  normalization = 0.0397436f * power +  0.0856832f;

	float  multiplier =  normalization * pow ( saturate( dot( n, h ) ) , power ) ;

    return multiplier * fresnel / max ( n_dot_l, n_dot_v );
}

shaded_surface gotanda(float3 albedo, float3 specular_color, float power, float3 l, float3 n, float3 v)
{
	shaded_surface result;
    float n_dot_l = dot ( n, l );

    if (n_dot_l > 0.0f)
    {
		n_dot_l = saturate(n_dot_l);
        result.m_kd = gotanda_diffuse ( albedo, l, n, n_dot_l, specular_color);
        result.m_ks = gotanda_specular ( specular_color, power, l, n, v, n_dot_l);
    }
    else
    {
        result.m_kd = float3(0.0f, 0.0f, 0.0f);
        result.m_ks = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

float3 schuller_diffuse(float3 albedo, float3 l, float3 n, float3 n_dot_l )
{
	const float pi = 3.14159265f;
    return (albedo * n_dot_l ) / pi;
}

float3 schuller_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float3 n_dot_l)
{
    float3 h = half_way_vector( l, v );

	const float pi = 3.14159265f;
	float  normalization = ( (power + 1.0f) / ( 8.0f * pi ) );

    float  multiplier =  normalization * pow ( saturate( dot( n, h ) ) , power ) ;
	float  l_dot_h = dot ( l, h);

    return n_dot_l * multiplier * specular_color / ( l_dot_h * l_dot_h * l_dot_h ) ;
}

shaded_surface schuller(float3 albedo, float3 specular_color, float power, float3 l, float3 n, float3 v)
{
	shaded_surface result;
    float n_dot_l = dot ( n, l );

    if (n_dot_l > 0.0f)
    {
		n_dot_l = saturate(n_dot_l);
        result.m_kd = schuller_diffuse ( albedo, l, n, n_dot_l);
        result.m_ks = schuller_specular ( specular_color, power, l, n, v, n_dot_l);
    }
    else
    {
        result.m_kd = float3(0.0f, 0.0f, 0.0f);
        result.m_ks = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

float3 main( in  vs_output input) : sv_target
{
    float2 uv               = input.uv;

	float3 kd               = diffuse_gbuffer.Sample(default_sampler, uv).xyz;
    float3 n_vs             = normalize( normal_gbuffer.Sample( default_sampler, uv ).xyz );
    float4 ks_gloss         = specular_gloss_gbuffer.Sample(default_sampler, uv);

    float depth_buffer_z    = depth_buffer.Sample(default_sampler, uv).x;

	//todo: fix with far plane to be at infinity
    if ( depth_buffer_z == 1.0f ) discard;

    float3 surface_sample_position_vs = convert_to_view_space ( input.uv, depth_buffer_z );

    float3 v						= -surface_sample_position_vs;

	float3 specular_color			= decode_specular_color( ks_gloss );
    float  specular_power			= 200.0f;//decode_specular_power( ks_gloss );

    float3 radiance					=  float3(0.0f,0.0f,0.0f);

    for (uint i = 0 ; i < m_light_count;++i)
    {
	    const float3 light_vs		= m_light_direction_vs[0].xyz;
		const float3 normal_vs		= n_vs;

        shaded_surface surface		= blinn_phong(kd, specular_color, specular_power, light_vs , normal_vs, normalize(v) );
        
        radiance		            = radiance +  decode_light_power ( m_light_color[i] ) * ( surface.m_kd  + surface.m_ks) ;
    }

    return radiance;
}


