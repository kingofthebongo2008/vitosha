cbuffer per_object
{
    float4    m_zn_zf;
};

struct vs_output
{
    float4	position_ps	    : sv_position; 
	float2	uv			    : texcoord; 
};

Texture2D		sampled_texture;
SamplerState	default_sampler;

float get_zn(float4 zn_zf)
{
    return zn_zf.x;
}

float get_zf(float4 zn_zf)
{
    return zn_zf.y;
}


float depth_buffer_z_2_linear_z(float depth_buffer_z, float a, float b)
{
    //if depth_buffer_z == 1.0f we get zf
    //if depth_buffer_z == 0.0f we get zn
    //in a should be stored ( - zf * zn / (zf - zn)
    //in b should be stored zf / (zf - zn)

    return b / ( depth_buffer_z - a );
}

//zn and zf are near and far planes from the camera settings
//depth_buffer_z holds non linear values [0;1]
float depth_buffer_z_2_linear_z_2(float depth_buffer_z, float zn, float zf)
{
    float a = zf / (zf - zn );
    float b = - ( zn * zf )  / (zf - zn );

    return b / ( depth_buffer_z - a );
}

//linear converts linear_z in [zn;zf] to [0;1] 
float normalize_linear_z(float linear_z, float zn, float zf)
{
    return ( linear_z - zn ) / ( zf - zn ); 
}

float4 main( in  vs_output input) : sv_target
{
	float depth_buffer_z = sampled_texture.Sample(default_sampler, input.uv).x;

    float zn = get_zn(m_zn_zf);
    float zf = get_zf(m_zn_zf);

    float linear_z = depth_buffer_z_2_linear_z_2( depth_buffer_z, zn, zf  );

    float normalized_linear_z = normalize_linear_z(linear_z, zn, zf );

    return float4( normalized_linear_z, normalized_linear_z, normalized_linear_z, 1.0f);
}