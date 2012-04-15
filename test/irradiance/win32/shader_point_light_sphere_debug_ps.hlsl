struct vs_output
{
    float4	position_ps				: sv_position;
	float4  sphere_surface_color	: sphere_color;
};

float3  main( in  vs_output input) : SV_Target
{
	return input.sphere_surface_color.xyz;
}

