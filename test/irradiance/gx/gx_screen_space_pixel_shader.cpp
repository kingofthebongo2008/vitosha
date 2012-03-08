#include "precompiled.h"
#include <gx/gx_screen_space_pixel_shader.h>

namespace gx
{
	#include "gx_shader_pixel_screen_space.h"

	screen_space_pixel_shader::screen_space_pixel_shader ( dx11::id3d11device_ptr device )
	{
		dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreatePixelShader( gx_shader_pixel_screen_space, sizeof(gx_shader_pixel_screen_space), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_pixel_screen_space[0];
		m_code_size = sizeof(gx_shader_pixel_screen_space);
	}
}