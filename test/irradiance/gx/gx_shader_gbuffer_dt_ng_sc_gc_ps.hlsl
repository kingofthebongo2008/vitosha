struct vs_output
{
    float4	position_ps	: sv_position;
	float3  normal_vs	: normal;
	float2	uv			: texcoord; 
};

cbuffer per_object
{
	float4	m_ks_gloss;
	bool	m_has_alpha_test;
}

Texture2D		diffuse;
SamplerState	default_sampler;

struct ps_output
{
	float3	m_kd			: sv_target0;
	float3	m_ks			: sv_target1;
    float4	m_normal_gloss	: sv_target2;
};

ps_output main( in  vs_output input)
{
	ps_output result;

	result.m_kd				= diffuse.Sample(default_sampler, input.uv).xyz;

	if (m_has_alpha_test)
	{
		clip(result.m_kd - 0.5f);
	}
	else
	{
		result.m_ks				= m_ks_gloss.xyz;
	    result.m_normal_gloss	= float4(normalize(input.normal_vs), m_ks_gloss.w);
	}

	return result;
}
