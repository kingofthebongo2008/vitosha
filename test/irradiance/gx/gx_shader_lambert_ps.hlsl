struct vs_output
{
    float4	position_ps	: sv_position; 
	float3  normal_ws	: normal;
	float2	uv			: texcoord; 
};

Texture2D		diffuse_albedo;
Texture2D		normal;

SamplerState	default_sampler;
float3			light_direction_ws;

float4 main( in  vs_output input) : sv_target
{
	//normal map is unfinished

	float3 f_1 = diffuse_albedo.Sample(default_sampler, input.uv).xyz;
	float3 f_2 = f_1 * saturate ( dot ( light_direction_ws, input.normal_ws ) );

	return float4(f_2, 1.0f);
}

