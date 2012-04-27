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
	float4	m_ks_gloss		: sv_target1;
    float3	m_normal		: sv_target2;
};

ps_output main( in  vs_output input)
{
   
	ps_output result;

	result.m_kd = m_kd.xyz; 
	result.m_ks_gloss = m_ks_gloss;
    result.m_normal = normalize(input.normal_vs);


    const float3		light_direction_vs = { -1.0f, 1.0f, 1.0f } ;
	const float3		light_power		   = { 3.1415f, 3.1415f, 3.1415f } ;	//watt

	float3 light_vs		= normalize(light_direction_vs);
	float3 normal_vs	= normalize(input.normal_vs);

	float3 diffuse_albedo = m_kd.xyz;

	float3 radiance		= diffuse_albedo.xyz * saturate(  ( dot ( light_vs, normal_vs ) ) );

    //result.m_normal = radiance;

	return result;
}
