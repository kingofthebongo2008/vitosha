#include "precompiled.h"
#include <gx/gx_encode_for_back_buffer_pixel_shader.h>

namespace gx
{
	#include "gx_shader_encode_for_back_buffer_ps_compiled.hlsl"

	encode_for_back_buffer_pixel_shader::encode_for_back_buffer_pixel_shader ( ID3D11Device* device )
	{
		dx::throw_if_failed<d3d11::create_pixel_shader> (device->CreatePixelShader( gx_shader_encode_for_back_buffer_ps, sizeof(gx_shader_encode_for_back_buffer_ps), nullptr, dx::get_pointer(m_shader)));
		m_code = &gx_shader_encode_for_back_buffer_ps[0];
		m_code_size = sizeof(gx_shader_encode_for_back_buffer_ps);
	}
}