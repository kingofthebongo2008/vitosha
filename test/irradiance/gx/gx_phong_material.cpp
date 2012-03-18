#include "precompiled.h"

#include <gx/gx_phong_material.h>

#include <dx11/dx11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>

namespace gx
{
	phong_texture_set::phong_texture_set (
							ID3D11Device*				device,
							dx11::id3d11texture2d_ptr	diffuse,
							dx11::id3d11texture2d_ptr	normal,
							dx11::id3d11texture2d_ptr	specular
					   )
	{
		dx11::throw_if_failed<dx11::create_resource_view_exception>(device->CreateShaderResourceView( diffuse.get(),  NULL, dx11::get_pointer(m_diffuse_view) ) );
		dx11::throw_if_failed<dx11::create_resource_view_exception>(device->CreateShaderResourceView( normal.get(),   NULL, dx11::get_pointer(m_normal_view) ) );
		dx11::throw_if_failed<dx11::create_resource_view_exception>(device->CreateShaderResourceView( specular.get(), NULL, dx11::get_pointer(m_specular_view) ));
	}


	void phong_material::apply(draw_call_context* draw_call_context)
	{
		ID3D11ShaderResourceView* resources[3] = { 
													m_texture_set.m_diffuse_view.get(),
													m_texture_set.m_normal_view.get(),
													m_texture_set.m_specular_view.get()
												 };

		draw_call_context->m_device_context->PSSetShaderResources(0, 3, resources );
	}

	phong_material::phong_material ( phong_texture_set texture_set, phong_shader_set shader_set ) : m_texture_set(texture_set), m_shader_set(shader_set)
	{
		m_material_id = gx::create_material_id ( this );
	}

	phong_material create_phong_material( ID3D11Device*	device, phong_texture_set texture_set )
	{
		phong_vertex_shader vs(device);
		phong_pixel_shader ps(device);
		phong_vertex_shader_constant_buffer cb(device);
		
		static phong_shader_set set( phong_vertex_shader(device), ps, phong_vertex_shader_constant_buffer(device) );

		return phong_material(texture_set, set);
	}

	phong_material create_phong_material( ID3D11Device*	device,  dx11::id3d11texture2d_ptr	diffuse, dx11::id3d11texture2d_ptr	normal, dx11::id3d11texture2d_ptr specular ) 
	{
		return create_phong_material(device, phong_texture_set( device, diffuse, normal, specular ) );
	}
}