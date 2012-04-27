struct vs_output
{
    float4	position_ps	: sv_position; 
	float3  normal_os	: normal;
	float2	uv			: texcoord; 
};

Texture2D		diffuse;
Texture2D		normal;
Texture2D		specular;

SamplerState	default_sampler;


struct ps_output
{
	float3	m_kd			: sv_target0;
	float4	m_ks_gloss		: sv_target2;
};

ps_output main( in  vs_output input)
{
    ps_output result;

	float3 kd = diffuse.Sample(default_sampler, input.uv).xyz;
    float3 normal_ts = normal.Sample(default_sampler, input.uv).xyz;
    float4 ks_gloss = specular.Sample(default_sampler, input.uv);

    result.m_kd = kd;
	result.m_ks_gloss = ks_gloss;

	return result;
}

