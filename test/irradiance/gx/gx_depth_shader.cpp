#include "precompiled.h"
#include <gx/gx_depth_shader.h>

namespace gx
{
	depth_vertex_shader_constant_buffer::depth_vertex_shader_constant_buffer ( dx11::id3d11device_ptr device )
	{
		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = sizeof(m_pvw);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, nullptr, dx11::get_pointer(m_buffer) ));
	}

	#include "gx_shader_depth.h"

	depth_vertex_shader::depth_vertex_shader ( dx11::id3d11device_ptr device )
	{
		dx11::throw_if_failed<dx11::create_vertex_shader> (device->CreateVertexShader( gx_shader_depth, sizeof(gx_shader_depth), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_depth[0];
		m_code_size = sizeof(gx_shader_depth);
	}
}