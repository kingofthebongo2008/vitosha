#include "precompiled.h"

#include "directional_lights.h"

#include <algorithm>
#include <iterator>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_helpers.h>

#include <math/math_graphics.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>


directional_lights_entity::directional_lights_entity
    ( 
        d3d11::ibuffer_ptr                  quad_data,
        gx::light_directional_vertex_pipeline   vertex_pipeline,
        gx::light_directional_pixel_pipeline    pixel_pipeline,
        const directional_light*                begin,
        const directional_light*                end
	) :
m_quad_data (quad_data)
, m_vertex_pipeline(vertex_pipeline)
, m_pixel_pipeline(pixel_pipeline)
{
    m_light_direction_ws.reserve(8);
    m_light_color.reserve(8);

    std::for_each ( begin, end, [= ] ( const directional_light& light ) 
    {
            m_light_direction_ws.push_back( light.get_power() );
            m_light_color.push_back ( light.get_direction() );
    }
    );
}

void directional_lights_entity::on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
{
	gx::draw_call_key key = gx::create_light_draw_call();
	collector->add_draw_call(key, context->m_entity_index ) ;
}


void directional_lights_entity::on_execute_draw_calls( gx::draw_call_context* context )
{
    auto device_context = context->m_device_context;

    std::get<1>(m_pixel_pipeline).set_inverse_projection(*context->m_inverse_projection_matrix);
    std::get<1>(m_pixel_pipeline).set_view(*context->m_view_matrix);

    std::get<1>(m_pixel_pipeline).set_light_count( static_cast<uint32_t> ( m_light_direction_ws.size() ) );
    std::get<1>(m_pixel_pipeline).set_light_direction_ws( &m_light_direction_ws[0], &m_light_direction_ws[0] + m_light_direction_ws.size() );
    std::get<1>(m_pixel_pipeline).set_light_color( &m_light_color[0], &m_light_color[0] + m_light_color.size() );

    std::get<1>(m_pixel_pipeline).flush(device_context);
    std::get<1>(m_pixel_pipeline).bind_as_pixel_constant_buffer(device_context);

    d3d11::vs_set_shader(device_context, std::get<0>(m_vertex_pipeline) );
	d3d11::ps_set_shader(device_context, std::get<0>(m_pixel_pipeline) );
	device_context->IASetInputLayout( std::get<1>(m_vertex_pipeline) );

    uint32_t stride = 12;
    uint32_t offset = 0;

    ID3D11Buffer*	buffers[] = { m_quad_data.get() } ;

	device_context->IASetVertexBuffers( 0, 1, &buffers[0], &stride, &offset );

    device_context->Draw( 6, 0 );

}

std::shared_ptr<directional_lights_entity> create_directional_lights_entity(    ID3D11Device* device,  d3d11::ibuffer_ptr quad_data, const directional_light* begin, const directional_light* end  )
{
    return std::make_shared<directional_lights_entity>( quad_data, gx::create_light_directional_vertex_pipeline(device), 
                                                        gx::create_light_directional_pixel_pipeline(device), begin, end );

}




