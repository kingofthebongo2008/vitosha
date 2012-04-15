struct vs_output
{
    float4	position_ps			 : sv_position;
	float4  sphere_surface_color : sphere_color;
	float3	normal_ws			 : normal;
};

float3  main( in  vs_output input) : SV_Target
{
	const float3		light_direction_ws = { 1.0f, 1.0f, 0.0f } ;
	const float3		light_power		   = { 3.1415f, 3.1415f, 3.1415f } ;	//watt

	float3 light_ws		= normalize(light_direction_ws);
	float3 normal_os	= normalize(input.normal_ws);

	float3 diffuse_albedo = input.sphere_surface_color.xyz;

	float3 radiance		= light_power * diffuse_albedo.xyz / 3.1415f * saturate(  ( dot ( light_ws, input.normal_ws ) ) );

	return radiance;
}

