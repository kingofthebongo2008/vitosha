#include "precompiled.h"

#include "floor.h"

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_helpers.h>

#include <math/math_half.h>
#include <math/math_graphics.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>
#include <gx/gx_lighting.h>
#include <gx/gx_shader_database.h>

void floor_entity::on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
{
    auto material_id = m_material_id;
    auto key = gx::create_gbuffer_draw_call(material_id, gx::get_perspective_transform3_depth(context), std::bind( &floor_entity::on_execute_draw_calls , this, std::placeholders::_1 ) );

    collector->add_draw_call(key, context->m_entity_index ) ;
}

void floor_entity::on_execute_draw_calls( gx::draw_call_context* draw_call_context )
{
    auto device_context = draw_call_context->m_device_context;

    std::get<1>(m_vertex_pipeline).set_w(*draw_call_context->m_world_matrix);
    std::get<1>(m_vertex_pipeline).set_normal_transform( math::mul( *draw_call_context->m_world_matrix, *draw_call_context->m_view_matrix ));
    std::get<1>(m_vertex_pipeline).flush(device_context);

    std::get<1>(m_vertex_pipeline).bind_as_vertex_constant_buffer(device_context);
    std::get<1>(m_vertex_pipeline).bind_as_geometry_constant_buffer(device_context);

    m_pixel_cbuffer.set_ks_gloss( math::set( 1.0f, 0.0f, 0.0f, gx::encode_specular_power(400.0f) ) );
    m_pixel_cbuffer.set_kd( math::set( 0.002f, 0.003f, 0.002f, 1.0f ) );
    m_pixel_cbuffer.flush(device_context);
    m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

    d3d11::vs_set_shader(device_context, std::get<0>(m_vertex_pipeline) );
    d3d11::gs_set_shader(device_context, m_geometry_shader );
    d3d11::ps_set_shader(device_context, m_pixel_shader );

    device_context->IASetInputLayout( std::get<2>(m_vertex_pipeline) );
    device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

    ID3D11Buffer* buffers[] = { m_point.get() };
    uint32_t stride = 8;
    uint32_t offset = 0;
    device_context->IASetVertexBuffers( 0, 1, buffers, &stride, &offset );
    device_context->Draw( 1, 0 );
   
    device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3d11::gs_set_shader(device_context, nullptr );
}

std::shared_ptr<floor_entity> create_floor_entity( ID3D11Device* device, const gx::shader_database* context, uint32_t width, uint32_t height, uint32_t subdivision_count )
{
    UNREFERENCED_PARAMETER(width);
    UNREFERENCED_PARAMETER(height);
    UNREFERENCED_PARAMETER(subdivision_count);

    math::half half[8] = {};

    d3d11::ibuffer_ptr point = d3d11::create_immutable_vertex_buffer(device, &half[0], sizeof(half) );
    d3d11::ibuffer_ptr out  = d3d11::create_stream_out_vertex_buffer(device, &half[0], 6 * sizeof(half) );


    std::tuple <gx::transform_position_normal_vertex_shader, gx::transform_position_normal_vertex_shader_constant_buffer,  gx::transform_position_normal_input_layout > 
    tupple(
                                std::move(context->m_transform_position_normal_vertex_shader),
                                std::move(context->m_transform_position_normal_vertex_shader_cbuffer),
                                std::move(context->m_transform_position_normal_input_layout)
        
        
    );

    return std::make_shared<floor_entity>
    (  
                                std::move(tupple),
                                gx::grid_geometry_shader(device) ,
                                context->m_blinn_phong_shift_invariant_pixel_shader,
                                context->m_blinn_phong_shift_invariant_pixel_cbuffer,
                                std::make_tuple< d3d11::ibuffer_ptr, d3d11::ibuffer_ptr > ( std::move(point),  std::move(out) )
    );
}




