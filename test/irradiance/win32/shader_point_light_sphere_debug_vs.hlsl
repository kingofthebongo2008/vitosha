
struct vs_input
{
	half4				position_os				: position;
	row_major float4x4	sphere_wvp				: sphere_transform;
	float4				sphere_surface_color	: sphere_color;
};

struct vs_output
{
    float4	position_ps	: sv_position;
	float4  sphere_surface_color : sphere_color;
};

vs_output main( in vs_input input)
{
	float4x4  wvp = input.sphere_wvp;

	vs_output output;

	output.position_ps = mul ( float4(input.position_os), wvp )  ;	
	output.sphere_surface_color = input.sphere_surface_color;
	return output;
}



