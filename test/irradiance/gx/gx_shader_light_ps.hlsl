struct vs_output
{
    float4	position_ps             : sv_position;
    float2  uv                      : texcoord;	
    float3	light_position			: light_position_vs; 
    float   light_radius            : light_radius;
    float4  light_color             : light_color;

};

Texture2D		diffuse_gbuffer;
Texture2D		normal_gbuffer;
Texture2D		specular_gloss_gbuffer;

Texture2D       depth_buffer;
SamplerState	default_sampler;

float4 main( in  vs_output input) : sv_target
{
	float3 f_1 = diffuse_gbuffer.Sample(default_sampler, input.uv).xyz;
    float3 f_2 = normal_gbuffer.Sample(default_sampler, input.uv).xyz;
    float4 f_3 = specular_gloss_gbuffer.Sample(default_sampler, input.uv);


    float3 normal_vs = f_2;

    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

