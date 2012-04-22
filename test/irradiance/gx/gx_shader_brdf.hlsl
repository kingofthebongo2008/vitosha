#ifndef __GX_SHADER_BRDF_HLSL_H__
#define __GX_SHADER_BRDF_HLSL_H__

struct blinn_phong_surface
{
	float3	m_kd;		// diffuse color
	float3	m_ks;		// (0.05 is for plastic for example, generally > 0.5 for metals ) . Dielectric materials do not have specular color, just metals
	float	m_gloss;	// gloss ( specular power = pow( l_max , m_gloss )  ( l_max is the maximum specular power in the game
};

float3 blinn_phong(float3 l, float3 v, float3 n, float3 kd, float3 ks, float alpha_p)
{
	float3 h = normalize(v+l);
	float3 c = ( kd + alpha_p * ks * pow( saturate( dot( h,n ) ),  alpha_p  ) ) * saturate( dot ( n,l ) );
	return c;
}


#endif

