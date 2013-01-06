#include "precompiled.h"
#include <gx/gx_gbuffer_dt_ng_sc_gc_pixel_shader.h>

namespace gx
{
	#include "gx_shader_gbuffer_dt_ng_sc_gc_ps_compiled.hlsl"

	gbuffer_dt_ng_sc_gc_pixel_shader::gbuffer_dt_ng_sc_gc_pixel_shader ( ID3D11Device* device )
	{
		dx::throw_if_failed<d3d11::create_pixel_shader> (device->CreatePixelShader( gx_shader_gbuffer_dt_ng_sc_gc_ps, sizeof(gx_shader_gbuffer_dt_ng_sc_gc_ps), nullptr, dx::get_pointer(m_shader)));
		m_code = &gx_shader_gbuffer_dt_ng_sc_gc_ps[0];
		m_code_size = sizeof(gx_shader_gbuffer_dt_ng_sc_gc_ps);
	}
}