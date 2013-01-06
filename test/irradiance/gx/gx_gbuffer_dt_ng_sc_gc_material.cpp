#include "precompiled.h"

#include <gx/gx_gbuffer_dt_ng_sc_gc_material.h>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_helpers.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>

namespace gx
{
	gbuffer_dt_ng_sc_gc_texture_set::gbuffer_dt_ng_sc_gc_texture_set (
							ID3D11Device*			device,
							d3d11::itexture2d_ptr	diffuse
					   )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc_diffuse = d3d11::create_srgb_shader_resource_view_desc(diffuse.get());
		dx::throw_if_failed<d3d11::create_resource_view_exception>(device->CreateShaderResourceView( diffuse.get(),  &desc_diffuse, dx::get_pointer(m_diffuse_view) ) );
	}


	void gbuffer_dt_ng_sc_gc_material::apply(draw_call_context* draw_call_context)
	{
        //set vertex shaders here also

		ID3D11ShaderResourceView* resources[1] = { 
													m_texture_set.m_diffuse_view.get()
												 };

        d3d11::ps_set_shader_resources ( draw_call_context->m_device_context, sizeof(resources) / sizeof( resources[0]) , resources );
	}

	gbuffer_dt_ng_sc_gc_material::gbuffer_dt_ng_sc_gc_material ( gbuffer_dt_ng_sc_gc_texture_set texture_set, gbuffer_dt_ng_sc_gc_shader_set shader_set, math::float4 ks_gloss ) : 
		m_texture_set(texture_set)
		, m_shader_set(shader_set)
		, m_ks_gloss(ks_gloss)
	{
		m_material_id = gx::create_material_id ( this );
	}

	gbuffer_dt_ng_sc_gc_material create_gbuffer_dt_ng_sc_gc_material( ID3D11Device*	device, gbuffer_dt_ng_sc_gc_texture_set texture_set, math::float4 ks_gloss )
	{
		return gbuffer_dt_ng_sc_gc_material
            (
                texture_set, 

                gbuffer_dt_ng_sc_gc_shader_set
                ( 
                    transform_position_normal_uv_vertex_shader(device),
                    gbuffer_dt_ng_sc_gc_pixel_shader(device),
                    transform_position_normal_uv_vertex_shader_constant_buffer(device)
                ),

				ks_gloss

            );
	}

	gbuffer_dt_ng_sc_gc_material create_gbuffer_dt_ng_sc_gc_material( ID3D11Device*	device,  d3d11::itexture2d_ptr	diffuse, math::float4 ks_gloss) 
	{
		return create_gbuffer_dt_ng_sc_gc_material(device, gbuffer_dt_ng_sc_gc_texture_set( device, diffuse ), ks_gloss );
	}
}