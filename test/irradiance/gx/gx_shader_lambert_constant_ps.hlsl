struct vs_output
{
    float4	position_ps	: sv_position;
	float3  normal_ws	: normal;
	float2	uv			: texcoord;
};

cbuffer per_object
{
	float4 m_diffuse_albedo;
}

float3 main( in  vs_output input) : SV_Target
{
	const float3		light_direction_ws = { -1.0f, 0.0f, 0.0f } ;
	const float3		light_power		   = { 3.1415f, 3.1415f, 3.1415f } ;	//watt

	float3 light_ws		= normalize(light_direction_ws);
	float3 normal_ws	= normalize(input.normal_ws);

	float3 radiance		= m_diffuse_albedo.xyz * saturate(  ( dot ( light_ws, normal_ws  ) ) );

	return radiance;
}
