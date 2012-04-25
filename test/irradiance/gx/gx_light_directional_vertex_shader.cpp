#include "precompiled.h"
#include <gx/gx_light_directional_vertex_shader.h>

namespace gx
{
	#include "gx_shader_light_directional_vs_compiled.hlsl"

	light_directional_vertex_shader::light_directional_vertex_shader ( ID3D11Device* device )
	{
		dx11::throw_if_failed<dx11::create_vertex_shader> (device->CreateVertexShader( gx_shader_light_directional_vs, sizeof(gx_shader_light_directional_vs), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_light_directional_vs[0];
		m_code_size = sizeof(gx_shader_light_directional_vs);
	}

    light_directional_vertex_shader_input_layout::light_directional_vertex_shader_input_layout( ID3D11Device* device, light_directional_vertex_shader* shader)
	{
	    D3D11_INPUT_ELEMENT_DESC desc[] = 
	    {
		    { "position",			0,	DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "texcoord",			0,  DXGI_FORMAT_R16G16_FLOAT,		1, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	    };

        dx11::throw_if_failed<dx11::create_input_layout> ( device->CreateInputLayout(&desc[0], sizeof(desc)/sizeof(desc[0]), shader->m_code, shader->m_code_size,  dx11::get_pointer(m_input_layout) ) );
	}

}