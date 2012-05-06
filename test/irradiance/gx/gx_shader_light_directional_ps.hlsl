cbuffer per_draw_call : register(c0)
{
    row_major float4x4 m_inverse_projection;
    row_major float4x4 m_view; 
    float4             m_light_direction_ws[8];
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

SamplerState	default_sampler : register(s0);


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

float3 rigid_transform_vector(float3 v, float4x4 m)
{
    //get rid of translation (assume no scaling)
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
    return pow ( 2, 10 * gloss + 1 );
}

float decode_specular_power( float4 ks_gloss )
{
    return decode_specular_power(ks_gloss.w) ;
}

float3 decode_specular_color( float4 ks_gloss )
{
    return ks_gloss.xyz;
}

float3 blinn_phong_specular(float3 specular_color, float power, float3 l, float3 n, float3 v)
{
    float3 h = half_way_vector( l, v );

    float3 fresnel = fresnel_schlick( specular_color, l, h );

    float  multiplier = ( (power + 2) / 8 )  * pow ( saturate( dot( n, h ) ) , power ) ;

    return multiplier * fresnel * saturate( dot (l, n) );
}

float3 blinn_phong_diffuse(float3 albedo, float3 l, float3 n )
{
    return albedo * saturate(  ( dot ( l, n ) ) );
}

float3 main( in  vs_output input) : sv_target
{

	float3 kd               = diffuse_gbuffer.Sample(default_sampler, input.uv).xyz;
    float3 n_vs             = normal_gbuffer.Sample( default_sampler, input.uv ).xyz;
    float4 ks_gloss         = specular_gloss_gbuffer.Sample(default_sampler, input.uv);

    float depth_buffer_z = depth_buffer.Sample(default_sampler, input.uv).x;

    if ( depth_buffer_z == 1.0f ) discard;

    float3 surface_sample_position_vs = convert_to_view_space ( input.uv, depth_buffer_z );

    float3 v            = surface_sample_position_vs;

    const float4		light_position_ws  = { 10.0f, 10.0f, 10.0f,  1.0f }; 

	float3 light_vs1	= normalize (  mul ( light_position_ws  , m_view ).xyz - surface_sample_position_vs   ) ;//rigid_transform_vector( normalize(light_position_ws) , m_view ) ;   //normalize(light_direction_vs);
	float3 normal_vs1	= n_vs;

    float3 diffuse		= blinn_phong_diffuse ( kd, light_vs1, n_vs) ;
    float3 specular		= blinn_phong_specular ( decode_specular_color( ks_gloss ), decode_specular_power(ks_gloss), light_vs1 , n_vs, normalize(v) ) ;

    float3 radiance		=  specular + diffuse;


    return radiance;
    
}


