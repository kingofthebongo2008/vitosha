struct vs_input
{
	half4	position_ps	        : position;
    half2   uv                  : texcoord;
};

struct vs_output
{
    float4  position_ps	        : sv_position; 
    float2  uv                  : texcoord;
};

float4 rigid_transform_vector(float3 v, float4x4 m)
{
    //get rid of translation (assume no scaling)
    float4x4 m_1 = m;
	m_1[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return mul ( v, m_1 );
}   

vs_output main( in  vs_input input)
{
    vs_output output;
	output.position_ps = float4(input.position_ps);
    output.uv = input.uv;

    /*
    output.light_direction_vs = rigid_transform_vector ( input.light_direction_ws.xyz, m_view) ;

    output.light_color =  input.light_color;

    */
	return output;
}
