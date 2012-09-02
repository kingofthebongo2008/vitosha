#include "precompiled.h"

#include <gx/gx_blinn_phong_material.h>

#include <d3d11/d3d11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>

namespace gx
{
	blinn_phong_texture_set::blinn_phong_texture_set (
							ID3D11Device*				device,
							d3d11::itexture2d_ptr	diffuse,
							d3d11::itexture2d_ptr	normal,
							d3d11::itexture2d_ptr	specular
					   )
	{
		dx::throw_if_failed<d3d11::create_resource_view_exception>(device->CreateShaderResourceView( diffuse.get(),  NULL, dx::get_pointer(m_diffuse_view) ) );
		dx::throw_if_failed<d3d11::create_resource_view_exception>(device->CreateShaderResourceView( normal.get(),   NULL, dx::get_pointer(m_normal_view) ) );
		dx::throw_if_failed<d3d11::create_resource_view_exception>(device->CreateShaderResourceView( specular.get(), NULL, dx::get_pointer(m_specular_view) ));
	}


	void blinn_phong_material::apply(draw_call_context* draw_call_context)
	{
        //set vertex shaders here also

		ID3D11ShaderResourceView* resources[3] = { 
													m_texture_set.m_diffuse_view.get(),
													m_texture_set.m_normal_view.get(),
													m_texture_set.m_specular_view.get()
												 };

        d3d11::ps_set_shader_resources ( draw_call_context->m_device_context, sizeof(resources) / sizeof( resources[0]) , resources );
	}

	blinn_phong_material::blinn_phong_material ( blinn_phong_texture_set texture_set, blinn_phong_shader_set shader_set ) : m_texture_set(texture_set), m_shader_set(shader_set)
	{
		m_material_id = gx::create_material_id ( this );
	}

	blinn_phong_material create_blinn_phong_material( ID3D11Device*	device, blinn_phong_texture_set texture_set )
	{
		return blinn_phong_material
            (
                texture_set, 

                blinn_phong_shader_set
                ( 
                    transform_position_normal_uv_vertex_shader(device),
                    blinn_phong_pixel_shader(device),
                    transform_position_normal_uv_vertex_shader_constant_buffer(device)
                ) 

            );
	}

	blinn_phong_material create_blinn_phong_material( ID3D11Device*	device,  d3d11::itexture2d_ptr	diffuse, d3d11::itexture2d_ptr	normal, d3d11::itexture2d_ptr specular ) 
	{
		return create_blinn_phong_material(device, blinn_phong_texture_set( device, diffuse, normal, specular ) );
	}
}