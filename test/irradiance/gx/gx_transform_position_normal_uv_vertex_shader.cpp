#include "precompiled.h"
#include <gx/gx_transform_position_normal_uv_vertex_shader.h>

namespace gx
{
	transform_position_normal_uv_vertex_shader_constant_buffer::transform_position_normal_uv_vertex_shader_constant_buffer ( ID3D11Device* device ) : 
        m_buffer( dx11::create_constant_buffer( device, size() )  )
	{
		
	}

	#include "gx_shader_transform_position_normal_uv_vs_compiled.hlsl"

	transform_position_normal_uv_vertex_shader::transform_position_normal_uv_vertex_shader ( ID3D11Device* device )
	{
		dx11::throw_if_failed<dx11::create_vertex_shader> (device->CreateVertexShader( gx_shader_transform_position_normal_uv_vs, sizeof(gx_shader_transform_position_normal_uv_vs), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_transform_position_normal_uv_vs[0];
		m_code_size = sizeof(gx_shader_transform_position_normal_uv_vs);
	}

    transform_position_normal_uv_input_layout::transform_position_normal_uv_input_layout( ID3D11Device* device, transform_position_normal_uv_vertex_shader* shader)
	{
       	D3D11_INPUT_ELEMENT_DESC desc[3] = 
		{
			{ "position",	0,	DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "normal",		0,	DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "texcoord",	0,  DXGI_FORMAT_R16G16_FLOAT, 1, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

        dx11::throw_if_failed<dx11::create_input_layout> ( device->CreateInputLayout(&desc[0], sizeof(desc)/sizeof(desc[0]), shader->m_code, shader->m_code_size,  dx11::get_pointer(m_input_layout) ) );
	}
}