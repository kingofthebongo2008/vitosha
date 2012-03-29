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
	const float3		light_direction_ws = { 0.0f, 0.0f, 1.0f } ;

	float3 light_ws		= normalize(light_direction_ws);

	float3 radiance		= m_diffuse_albedo * ( dot ( light_ws, input.normal_ws ) );

	return radiance;
}
