#include "precompiled.h"
#include <gx/gx_phong_pixel_shader.h>

namespace gx
{
	#include "gx_shader_phong.psh.h"

	phong_pixel_shader::phong_pixel_shader ( ID3D11Device* device )
	{
		dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreatePixelShader( gx_shader_phong_psh, sizeof(gx_shader_phong_psh), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_phong_psh[0];
		m_code_size = sizeof(gx_shader_phong_psh);
	}
}