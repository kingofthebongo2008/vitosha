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

vs_output main( in  vs_input input)
{
    vs_output output;
	output.position_ps = float4(input.position_ps);
    output.uv = float2(input.uv);
	return output;
}
