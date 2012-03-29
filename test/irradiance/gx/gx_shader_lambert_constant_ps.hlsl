struct vs_output
{
    float4	position_ps	: SV_POSITION; 
	float3  normal_ws	: Normal0;
	float2	uv			: TexCoord0; 
};

cbuffer per_object
{
	float4 m_diffuse_albedo;
}

float3 main( in  vs_output input) : SV_Target
{
	const float3		light_direction_ws = { 1.0f, 1.0f, 0.0f } ;
	const float3		light_power		   = { 3.0f, 3.0f, 3.0f } ;	//watt

	float3 light_ws		= normalize(light_direction_ws);

	float3 radiance		= light_power * m_diffuse_albedo / 3.1415f * saturate(  ( dot ( light_ws, input.normal_ws ) ) );

	return radiance;
}
