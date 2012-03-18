#include "precompiled.h"
#include <gx/gx_phong_vertex_shader.h>

namespace gx
{
	phong_vertex_shader_constant_buffer::phong_vertex_shader_constant_buffer ( ID3D11Device* device )
	{
		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = sizeof(m_wvp);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, nullptr, dx11::get_pointer(m_buffer) ));
	}

	#include "gx_shader_phong.vsh.h"

	phong_vertex_shader::phong_vertex_shader ( ID3D11Device* device )
	{
		dx11::throw_if_failed<dx11::create_vertex_shader> (device->CreateVertexShader( gx_shader_phong_vsh, sizeof(gx_shader_phong_vsh), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_phong_vsh[0];
		m_code_size = sizeof(gx_shader_phong_vsh);
	}
}