#include "precompiled.h"
#include <gx/gx_screen_space_pixel_shader.h>

namespace gx
{
	#include "gx_shader_screen_space_ps_compiled.hlsl"

	screen_space_pixel_shader::screen_space_pixel_shader ( ID3D11Device* device )
	{
		dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreatePixelShader( gx_shader_screen_space_ps, sizeof(gx_shader_screen_space_ps), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_screen_space_ps[0];
		m_code_size = sizeof(gx_shader_screen_space_ps);
	}
}