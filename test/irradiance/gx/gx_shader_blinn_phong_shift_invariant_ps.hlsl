struct vs_output
{
    float4	position_ps	: sv_position;
	float3  normal_vs	: normal;
};

cbuffer per_object
{
	float4	m_kd;
	float4	m_ks_gloss;
}

struct ps_output
{
	float3	m_kd			: sv_target0;
	float3	m_ks_gloss		: sv_target1;
    float4	m_normal_gloss	: sv_target2;
};

ps_output main( in  vs_output input)
{
   
	ps_output result;

	result.m_kd = m_kd.xyz; 
	result.m_ks_gloss = m_ks_gloss.xyz;
    result.m_normal_gloss = float4(normalize(input.normal_vs), m_ks_gloss.w);

	return result;
}
