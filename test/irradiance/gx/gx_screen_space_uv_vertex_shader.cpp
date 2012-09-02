#include "precompiled.h"
#include <gx/gx_screen_space_uv_vertex_shader.h>

namespace gx
{
	screen_space_uv_vertex_shader_constant_buffer::screen_space_uv_vertex_shader_constant_buffer ( ID3D11Device* device ) : 
		m_buffer( d3d11::create_constant_buffer( device, size() )  )
	{
		
	}

	#include "gx_shader_screen_space_uv_vs_compiled.hlsl"

	screen_space_uv_vertex_shader::screen_space_uv_vertex_shader ( ID3D11Device* device )
	{
		dx::throw_if_failed<d3d11::create_vertex_shader> (device->CreateVertexShader( gx_shader_screen_space_uv_vs, sizeof(gx_shader_screen_space_uv_vs), nullptr, dx::get_pointer(m_shader)));
		m_code = &gx_shader_screen_space_uv_vs[0];
		m_code_size = sizeof(gx_shader_screen_space_uv_vs);
	}

}