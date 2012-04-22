struct vs_output
{
    float4	position_ps	: sv_position; 
	float3  normal_os	: normal;
	float2	uv			: texcoord; 
};

Texture2D		diffuse;
Texture2D		normal;
Texture2D		specular;

SamplerState	default_sampler;
float3			light_direction_ws;

float4 main( in  vs_output input) : SV_Target
{
	float3 f_1 = diffuse.Sample(default_sampler, input.uv).xyz;
	float3 f_2 = f_1 * saturate ( dot ( light_direction_ws, input.normal_os ) );

	return float4(f_2, 1.0f);
}

