#include "precompiled.h"

#include <gx/gx_lambert_material.h>

#include <d3d11/d3d11_helpers.h>

#include <gx/gx_draw_call_context.h>

namespace gx
{
	lambert_texture_set::lambert_texture_set (
							ID3D11Device*				device,
							d3d11::itexture2d_ptr	diffuse,
							d3d11::itexture2d_ptr	normal
					   )
	{
		dx::throw_if_failed<d3d11::create_resource_view_exception>(device->CreateShaderResourceView( diffuse.get(),  NULL, dx::get_pointer(m_diffuse_view) ) );
		dx::throw_if_failed<d3d11::create_resource_view_exception>(device->CreateShaderResourceView( normal.get(),   NULL, dx::get_pointer(m_normal_view) ) );
	}


	void lambert_material::apply(draw_call_context* draw_call_context)
	{
		ID3D11ShaderResourceView* resources[2] = { 
													m_texture_set.m_diffuse_view.get(),
													m_texture_set.m_normal_view.get()
												};

        d3d11::ps_set_shader_resources ( draw_call_context->m_device_context, sizeof(resources) / sizeof(resources[2]), resources );
	}
}