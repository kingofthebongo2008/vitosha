struct vs_output
{
    float4	position_ps	: sv_position; 
	float2	uv			: texcoord; 
};

Texture2D		sampled_texture;
SamplerState	default_sampler;

float main( in  vs_output input) : sv_target
{
    //read from the depth buffer and output in r32 target
	float f_1 = sampled_texture.Sample(default_sampler, input.uv).x;
	return f_1;
}
