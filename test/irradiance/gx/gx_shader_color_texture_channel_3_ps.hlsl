struct vs_output
{
    float4	position_ps	: sv_position; 
	float2	uv			: texcoord; 
};

Texture2D		sampled_texture;
SamplerState	default_sampler;

float4 main( in  vs_output input) : sv_target
{
    
	float f_1 = sampled_texture.Sample(default_sampler, input.uv).w;
	return float4( f_1, f_1, f_1, f_1 );
    

    /*
    float f_1 = sampled_texture.Sample(default_sampler, input.uv).x;
    float near = 0.1f; //meters
    float far = 400.0f;	//meters;

    float f_2 = 2 * near / ( far + near - f_1 * ( far - near ) );

	return float4( f_2, f_2, f_2, f_2 );
    */
}
