cbuffer per_object
{
	float4x4 m_pvw;
}

struct vs_input
{
	float3	position	: POSITION;
	float2	uv			: TexCoord;
};

struct vs_output
{
    float4	position	: SV_POSITION; 
	float2	uv			: TexCoord; 
};

vs_output main( in  vs_input input)
{
	vs_output output;
	output.position = mul ( m_pvw, float4(input.position, 1.0f) )  ;	
	output.uv = input.uv;
	return output;
}





