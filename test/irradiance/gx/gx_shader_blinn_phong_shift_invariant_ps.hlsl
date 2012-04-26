struct vs_output
{
    float4	position_ps	: sv_position;
	float3  normal_ws	: normal;
};

cbuffer per_object
{
	float4	m_kd;
	float4	m_ks_gloss;		
}

struct ps_output
{
	float3	m_kd			: sv_target0;
	float4	m_ks_gloss		: sv_target2;
    float3	m_normal		: sv_target1;
};

ps_output main( in  vs_output input)
{
	ps_output result;

	result.m_kd = m_kd; 
	result.m_ks_gloss = m_ks_gloss;
    result.m_normal = input.normal_ws;

	return result;
}
