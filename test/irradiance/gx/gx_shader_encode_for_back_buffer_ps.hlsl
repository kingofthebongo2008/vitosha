struct vs_output
{
    float4	position_ps	: sv_position; 
	float2	uv			: texcoord; 
};

Texture2D		sampled_texture;
SamplerState	point_sampler;

float4 main( in  vs_output input) : sv_target
{
	float4 f_1 = sampled_texture.Sample(point_sampler, input.uv);
	return pow( abs(f_1) , 1/2.2f);
}