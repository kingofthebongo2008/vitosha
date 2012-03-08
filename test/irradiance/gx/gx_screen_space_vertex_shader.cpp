#include "precompiled.h"
#include <gx/gx_screen_space_vertex_shader.h>

namespace gx
{
	screen_space_vertex_shader_constant_buffer::screen_space_vertex_shader_constant_buffer ( dx11::id3d11device_ptr device )
	{
		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = sizeof(m_wvp);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, nullptr, dx11::get_pointer(m_buffer) ));
	}

	#include "gx_shader_vertex_screen_space.h"

	screen_space_vertex_shader::screen_space_vertex_shader ( dx11::id3d11device_ptr device )
	{
		dx11::throw_if_failed<dx11::create_vertex_shader> (device->CreateVertexShader( gx_shader_vertex_screen_space, sizeof(gx_shader_vertex_screen_space), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_vertex_screen_space[0];
		m_code_size = sizeof(gx_shader_vertex_screen_space);
	}
}