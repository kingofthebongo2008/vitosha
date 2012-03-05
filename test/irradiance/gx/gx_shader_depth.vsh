cbuffer per_object
{
	float4x4 m_pvw;
}

struct VS_INPUT
{
	float3	Position	: POSITION;
};

struct VS_OUTPUT
{
    float4	Position	: SV_POSITION; 
};

VS_OUTPUT main( in  VS_INPUT Input)
{
	VS_OUTPUT output;
	output.Position = mul ( m_pvw, float4(Input.Position, 1.0f) )  ;	
	return output;
}



