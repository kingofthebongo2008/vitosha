#include "precompiled.h"
#include <gx/gx_blinn_phong_shift_invariant_pixel_shader.h>

namespace gx
{
	#include "gx_shader_blinn_phong_shift_invariant_ps_compiled.hlsl"

	blinn_phong_shift_invariant_pixel_shader_constant_buffer::blinn_phong_shift_invariant_pixel_shader_constant_buffer ( ID3D11Device* device ) :
        m_buffer( dx11::create_constant_buffer( device, size() )  )
	{
	}

	blinn_phong_shift_invariant_pixel_shader::blinn_phong_shift_invariant_pixel_shader ( ID3D11Device* device )
	{
		dx11::throw_if_failed<dx11::create_pixel_shader> (device->CreatePixelShader( gx_shader_blinn_phong_shift_invariant_ps, sizeof(gx_shader_blinn_phong_shift_invariant_ps), nullptr, dx11::get_pointer(m_shader)));
		m_code = &gx_shader_blinn_phong_shift_invariant_ps[0];
		m_code_size = sizeof(gx_shader_blinn_phong_shift_invariant_ps);
	}
}