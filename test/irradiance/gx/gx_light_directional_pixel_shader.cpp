#include "precompiled.h"
#include <gx/gx_light_directional_pixel_shader.h>

namespace gx
{
	#include "gx_shader_light_directional_ps_compiled.hlsl"

	light_directional_pixel_shader_constant_buffer::light_directional_pixel_shader_constant_buffer ( ID3D11Device* device ) :
        m_buffer( d3d11::create_constant_buffer( device, size() )  )
	{

	}

	light_directional_pixel_shader::light_directional_pixel_shader ( ID3D11Device* device )
	{
		dx::throw_if_failed<d3d11::create_pixel_shader> (device->CreatePixelShader( gx_shader_light_directional_ps, sizeof(gx_shader_light_directional_ps), nullptr, dx::get_pointer(m_shader)));
		m_code = &gx_shader_light_directional_ps[0];
		m_code_size = sizeof(gx_shader_light_directional_ps);
	}
}