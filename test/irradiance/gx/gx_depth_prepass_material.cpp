#include "precompiled.h"

#include <gx/gx_depth_prepass_material.h>

#include <d3d11/d3d11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>
#include <gx/gx_shader_database.h>

namespace gx
{
    void depth_prepass_material::apply(draw_call_context* draw_call_context)
    {
        ID3D11DeviceContext* device_context = draw_call_context->m_device_context;

        m_vertex_cbuffer.update(device_context, draw_call_context->m_world_matrix);

        m_vertex_cbuffer.bind_as_vertex_constant_buffer(device_context);

        d3d11::vs_set_shader(device_context, m_vertex_shader );
        device_context->IASetInputLayout(m_input_layout);
    }

    depth_prepass_material::depth_prepass_material ( 
                                        transform_position_vertex_shader                    vertex_shader,
                                        transform_position_vertex_shader_constant_buffer    vertex_cbuffer,
                                        transform_position_input_layout                     input_layout
        ) :   m_vertex_shader(vertex_shader)
             , m_vertex_cbuffer(vertex_cbuffer)
             , m_input_layout(input_layout)
    {
    }

    depth_prepass_material create_depth_prepass_material( const shader_database* context)
    {
        return depth_prepass_material( 
                                context->m_transform_position_vertex_shader,
                                context->m_transform_position_vertex_shader_cbuffer,
                                context->m_transform_position_input_layout);
    }
}