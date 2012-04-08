#include "precompiled.h"
#include <gx/gx_lambert_constant_pixel_shader.h>

namespace gx
{
	#include "gx_shader_lambert_constant_ps_compiled.hlsl"

	lambert_constant_pixel_shader_constant_buffer::lambert_constant_pixel_shader_constant_buffer ( ID3D11Device* device )
	{
		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = sizeof(m_diffuse_albedo);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		dx11::throw_if_failed<dx11::create_buffer_exception> (device->CreateBuffer(&desc, nullptr, dx11::get_pointer(m_buffer) ));
	}

	lambert_constant_pixel_shader::lambert_constant_pixel_shader ( ID3D11Device* device )
	{
		dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreatePixelShader( gx_shader_lambert_constant_ps, sizeof(gx_shader_lambert_constant_ps), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_lambert_constant_ps[0];
		m_code_size = sizeof(gx_shader_lambert_constant_ps);
	}
}