#ifndef __GX_SHADER_LIGHTING_H__
#define __GX_SHADER_LIGHTING_H__

#include "gx_shader_geometry_pass_common.hlsl"

struct shaded_surface
{
    float3 m_kd;
    float3 m_ks;
};

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

float decode_light_range( float4 light)
{
    return light.w;
}

float3 blinn_phong_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float n_dot_l)
{
    float3 h = half_way_vector( l, v );

    float3 fresnel = fresnel_schlick( specular_color, l, h );

	const float pi = 3.14159265f;
	float  normalization = ( (power + 2.0f) / ( 8.0f ) ); //( (power + 8.0f) / ( 8.0f * pi ) );
	//float  normalization = ( (power + 8.0f) / ( 8.0f * pi ) ); //( (power + 8.0f) / ( 8.0f * pi ) );

    float  multiplier =  normalization * pow ( saturate( dot( n, h ) ) , power ) ;

    return multiplier * fresnel * n_dot_l;
}

float3 blinn_phong_diffuse(float3 albedo, float3 l, float3 n, float n_dot_l )
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
        result.m_ks = blinn_phong_specular ( specular_color, power, l, n, v, n_dot_l);
    }
    else
    {
        result.m_kd = float3(0.0f, 0.0f, 0.0f);
        result.m_ks = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

float3 phong_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float n_dot_l)
{
    float3 h = half_way_vector( l, v );

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
        result.m_ks = phong_specular ( specular_color, power / 4.0f, l, n, v, n_dot_l);
    }
    else
    {
        result.m_kd = float3(0.0f, 0.0f, 0.0f);
        result.m_ks = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

float3 gotanda_diffuse(float3 albedo, float3 l, float3 n, float n_dot_l, float3 specular_color )
{
	const float pi = 3.14159265f;
	
	//float3 fresnel = fresnel_schlick( specular_color, n, l );
    //return n_dot_l * ( albedo  *  ( 1.0f - fresnel ) ) / pi;
	

	//approximation
	return n_dot_l * ( albedo * (1.0f - specular_color ) ) /  pi;
}

float3 gotanda_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float n_dot_l)
{
    float3 h = half_way_vector( l, v );
	float  n_dot_v  = dot ( n, v );

    float3 fresnel = fresnel_schlick( specular_color, l, h );

	float  normalization = 0.0397436f * power +  0.0856832f;

	float  multiplier =  normalization * pow ( saturate( dot( n, h ) ) , power ) ;

    return n_dot_l * multiplier * fresnel / ( max ( n_dot_l, n_dot_v ) );
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

float3 schuller_diffuse(float3 albedo, float3 l, float3 n, float n_dot_l )
{
	const float pi = 3.14159265f;
    return (albedo * n_dot_l ) / pi;
}

float3 schuller_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float n_dot_l)
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


float3 cook_torrance_diffuse(float3 albedo, float3 l, float3 n, float n_dot_l )
{
	const float pi = 3.14159265f;
    return (albedo * n_dot_l ) / pi;
}

float3 cook_torrance_specular(float3 specular_color, float power, float3 l, float3 n, float3 v, float n_dot_l)
{
    float3 h = half_way_vector( l, v );

    float3 fresnel = fresnel_schlick( specular_color, l, h );

	float n_dot_h = dot (n, h );
	float n_dot_v = dot (n, v );
	float n_dot_h_2 = n_dot_h * n_dot_h;
	float v_dot_h = dot ( v, h );

	const float pi = 3.14159265f;
	float m = sqrt ( 2.0f / (power + 2.0f) );

	float normalization = 1.0f / ( 4 * n_dot_l * n_dot_v );
    float beckmann = exp( -1 * ( 1 - n_dot_h_2 ) / ( m * m * n_dot_h_2 ) ) / ( pi * m * m * n_dot_h_2 * n_dot_h_2 );
	//float phong = ( (power + 2.0) / 2.0*pi )  * pow ( n_dot_h, power );

	float visibility = min ( 1.0f, min ( 2 * n_dot_h * n_dot_v / v_dot_h, 2 * n_dot_h * n_dot_l / v_dot_h ) );


    return n_dot_l * fresnel * ( beckmann * visibility * normalization );
}

shaded_surface cook_torrance(float3 albedo, float3 specular_color, float power, float3 l, float3 n, float3 v)
{
	shaded_surface result;
    float n_dot_l = dot ( n, l );

    if (n_dot_l > 0.0f)
    {
		n_dot_l = saturate(n_dot_l);
		result.m_ks = cook_torrance_specular ( specular_color, power, l, n, v, n_dot_l);
        result.m_kd = (1 - result.m_ks) * cook_torrance_diffuse ( albedo, l, n, n_dot_l);
    }
    else
    {
        result.m_kd = float3(0.0f, 0.0f, 0.0f);
        result.m_ks = float3(0.0f, 0.0f, 0.0f);
    }

    return result;
}

//distance and range are in meters
float light_attenuation(float distance, float range, float strength)
{
	float attenuation = (distance * distance) / (range * range);

	attenuation = 1.0f / (attenuation * strength + 1.f);
	strength = 1.0f / (strength + 1.0f);
	attenuation = attenuation - strength;
	attenuation = attenuation / (1.0f - strength );

	return attenuation;
}

//distance and range are in meters
float light_attenuation_2(float distance_squared, float range_squared, float strength)
{
	float attenuation = (distance_squared) / (range_squared);

	attenuation = 1.0f / (attenuation * strength + 1.f);
	strength = 1.0f / (strength + 1.0f);
	attenuation = attenuation - strength;
	attenuation = attenuation / (1.0f - strength );

	return attenuation;
}


#endif