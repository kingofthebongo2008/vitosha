struct vs_output
{
    float4	position_ps	: sv_position; 
	float2	uv			: texcoord; 
};

Texture2D		sampled_texture;
SamplerState	default_sampler;

float4 main( in  vs_output input) : sv_target
{
	float4 f_1 = sampled_texture.Sample(default_sampler, input.uv);
	return f_1;
}