#include "precompiled.h"

#include <gx/gx_lambert_material.h>

#include <dx11/dx11_error.h>

#include <gx/gx_draw_call_context.h>

namespace gx
{
	lambert_texture_set::lambert_texture_set (
							ID3D11Device*				device,
							dx11::id3d11texture2d_ptr	diffuse,
							dx11::id3d11texture2d_ptr	normal
					   )
	{
		dx11::throw_if_failed<dx11::create_resource_view_exception>(device->CreateShaderResourceView( diffuse.get(),  NULL, dx11::get_pointer(m_diffuse_view) ) );
		dx11::throw_if_failed<dx11::create_resource_view_exception>(device->CreateShaderResourceView( normal.get(),   NULL, dx11::get_pointer(m_normal_view) ) );
	}


	void lambert_material::apply(draw_call_context* draw_call_context)
	{
		ID3D11ShaderResourceView* resources[2] = { 
													m_texture_set.m_diffuse_view.get(),
													m_texture_set.m_normal_view.get()
												};

		draw_call_context->m_device_context->PSSetShaderResources(0, 2, resources );
	}
}