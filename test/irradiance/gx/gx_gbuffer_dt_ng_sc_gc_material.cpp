#include "precompiled.h"

#include <gx/gx_gbuffer_dt_ng_sc_gc_material.h>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_helpers.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>
#include <gx/gx_shader_database.h>

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


        auto device_context = draw_call_context->m_device_context;

        std::get<1>(m_shader_set.m_vertex_pipeline).set_w(*draw_call_context->m_world_matrix);
        std::get<1>(m_shader_set.m_vertex_pipeline).set_normal_transform( math::transpose( math::inverse( math::mul( *draw_call_context->m_world_matrix, *draw_call_context->m_view_matrix ))));

        std::get<1>(m_shader_set.m_vertex_pipeline).flush(device_context);
        std::get<1>(m_shader_set.m_vertex_pipeline).bind_as_vertex_constant_buffer(device_context);

        m_shader_set.m_pixel_cbuffer.set_ks_gloss(m_ks_gloss);
        m_shader_set.m_pixel_cbuffer.set_has_alpha_test(m_has_alpha_test);

        m_shader_set.m_pixel_cbuffer.flush(device_context);
        m_shader_set.m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

        d3d11::vs_set_shader(device_context, std::get<0>(m_shader_set.m_vertex_pipeline) );
        d3d11::ps_set_shader(device_context, m_shader_set.m_pixel_shader );
        device_context->IASetInputLayout( std::get<2>(m_shader_set.m_vertex_pipeline) );
    }

    gbuffer_dt_ng_sc_gc_material::gbuffer_dt_ng_sc_gc_material ( gbuffer_dt_ng_sc_gc_texture_set texture_set, gbuffer_dt_ng_sc_gc_shader_set shader_set, math::float4 ks_gloss, bool has_alpha_test ) : 
        m_texture_set(texture_set)
        , m_shader_set(shader_set)
        , m_ks_gloss(ks_gloss)
        , m_has_alpha_test(has_alpha_test)
    {
        m_material_id = gx::create_material_id ( this );
    }

    gbuffer_dt_ng_sc_gc_material create_gbuffer_dt_ng_sc_gc_material( const shader_database* context, gbuffer_dt_ng_sc_gc_texture_set texture_set, math::float4 ks_gloss, bool has_alpha_test )
    {
        std::tuple< transform_position_normal_uv_vertex_shader, transform_position_normal_uv_vertex_shader_constant_buffer, transform_position_normal_uv_input_layout >
            tuple(
                    std::move(context->m_transform_position_normal_uv_vertex_shader),
                    std::move(context->m_transform_position_normal_uv_vertex_shader_cbuffer),
                    std::move(context->m_transform_position_normal_uv_input_layout)
            
            );


        return gbuffer_dt_ng_sc_gc_material
            (
                texture_set, 

                gbuffer_dt_ng_sc_gc_shader_set
                ( 
                    std::move(tuple),
                    context->m_gbuffer_dt_ng_sc_gc_pixel_shader,
                    context->m_gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer
                ),

                ks_gloss,
                has_alpha_test

            );
    }

    gbuffer_dt_ng_sc_gc_material create_gbuffer_dt_ng_sc_gc_material( ID3D11Device* device, const shader_database* context, d3d11::itexture2d_ptr diffuse, math::float4 ks_gloss, bool has_alpha_test) 
    {
        return create_gbuffer_dt_ng_sc_gc_material(context, gbuffer_dt_ng_sc_gc_texture_set( device, diffuse ), ks_gloss, has_alpha_test );
    }
}