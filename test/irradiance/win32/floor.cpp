#include "precompiled.h"

#include "floor.h"

#include <dx11/dx11_error.h>
#include <dx11/dx11_helpers.h>

#include <math/math_half.h>
#include <math/math_graphics.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>
#include <gx/gx_render_context.h>

void floor_entity::on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
{
	auto material_id = m_material_id;
    auto key = gx::create_gbuffer_draw_call(material_id, gx::get_perspective_transform3_depth(context) );

    collector->add_draw_call(key, context->m_entity_index ) ;
}

void floor_entity::on_execute_draw_calls( gx::draw_call_context* draw_call_context )
{
    auto device_context = draw_call_context->m_device_context;

    std::get<1>(m_vertex_pipeline).set_wvp(*draw_call_context->m_wvp_matrix);
    std::get<1>(m_vertex_pipeline).set_normal_transform( math::mul( *draw_call_context->m_world_matrix, *draw_call_context->m_view_matrix ));

    std::get<1>(m_vertex_pipeline).flush(device_context);
    std::get<1>(m_vertex_pipeline).bind_as_geometry_constant_buffer(device_context);

    m_pixel_cbuffer.set_ks_gloss( math::set( 1.0f, 0.3f, 1.0f, 0.2f ) );
    m_pixel_cbuffer.set_kd( math::set( 0.2f, 0.3f, 1.0f, 1.0f ) );
	m_pixel_cbuffer.flush(device_context);
    m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

    dx11::vs_set_shader(device_context, std::get<0>(m_vertex_pipeline) );
    dx11::gs_set_shader(device_context, m_geometry_shader );
	dx11::ps_set_shader(device_context, m_pixel_shader );

    device_context->IASetInputLayout( std::get<2>(m_vertex_pipeline) );
    //device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

    ID3D11Buffer* buffers[] = { m_point.get() };
	uint32_t stride = 8;
    uint32_t offset = 0;
	device_context->IASetVertexBuffers( 0, 1, buffers, &stride, &offset );
	device_context->Draw( 1, 0 );
   
    //device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    dx11::gs_set_shader(device_context, nullptr );
}

std::shared_ptr<floor_entity> create_floor_entity( ID3D11Device* device, gx::render_context* context, uint32_t width, uint32_t height, uint32_t subdivision_count )
{
    math::half half[8] = {};

    dx11::id3d11buffer_ptr point = dx11::create_immutable_vertex_buffer(device, &half[0], sizeof(half) );
    dx11::id3d11buffer_ptr out  = dx11::create_stream_out_vertex_buffer(device, &half[0], 6 * sizeof(half) );


    return std::make_shared<floor_entity>
    (  
        std::make_tuple< gx::transform_position_normal_vertex_shader, gx::transform_position_normal_vertex_shader_constant_buffer,  gx::transform_position_normal_input_layout > 
                                ( 
                                    std::move(context->m_transform_position_normal_vertex_shader),
                                    std::move(context->m_transform_position_normal_vertex_shader_cbuffer),
                                    std::move(context->m_transform_position_normal_input_layout)
                                ),
                                gx::grid_geometry_shader(device) ,
								context->m_blinn_phong_shift_invariant_pixel_shader,
								context->m_blinn_phong_shift_invariant_pixel_cbuffer,
                                std::make_tuple< dx11::id3d11buffer_ptr, dx11::id3d11buffer_ptr > ( std::move(point),  std::move(out) )
                                );
}




