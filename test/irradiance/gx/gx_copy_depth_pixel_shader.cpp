#include "precompiled.h"
#include <gx/gx_copy_depth_pixel_shader.h>

namespace gx
{
	#include "gx_shader_copy_depth_ps_compiled.hlsl"

	copy_depth_pixel_shader::copy_depth_pixel_shader ( ID3D11Device* device )
	{
		dx::throw_if_failed<d3d11::create_pixel_shader> (device->CreatePixelShader( gx_shader_copy_depth_ps, sizeof(gx_shader_copy_depth_ps), nullptr, dx::get_pointer(m_shader)));
		m_code = &gx_shader_copy_depth_ps[0];
		m_code_size = sizeof(gx_shader_copy_depth_ps);
	}
}