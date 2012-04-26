#include "precompiled.h"

#include "directional_lights.h"

#include <algorithm>
#include <iterator>

#include <dx11/dx11_error.h>
#include <dx11/dx11_helpers.h>

#include <math/math_graphics.h>

#include <gx/gx_draw_call_collector.h>
#include <gx/gx_draw_call_collector_context.h>
#include <gx/gx_draw_call_context.h>


directional_lights_entity::directional_lights_entity
    ( 
        dx11::id3d11buffer_ptr                  quad_data,
        gx::light_directional_vertex_pipeline   vertex_pipeline,
        gx::light_directional_pixel_pipeline    pixel_pipeline,
        const directional_light*                begin,
        const directional_light*                end
	) :
m_quad_data (quad_data)
, m_vertex_pipeline(vertex_pipeline)
, m_pixel_pipeline(pixel_pipeline)
{
    m_light_position_ws.reserve(8);
    m_light_color.reserve(8);

    std::for_each ( begin, end, [= ] ( const directional_light& light ) 
    {
            m_light_position_ws.push_back( light.get_power() );
            m_light_color.push_back ( light.get_direction() );
    }
    );
}

void directional_lights_entity::on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
{
	gx::draw_call_key key = gx::create_light_draw_call();
	collector->add_draw_call(key, context->m_entity_index ) ;
}

/*
        void set_inverse_projection(math::matrix_float44 value)
		{
			m_inverse_projection = value;
		}

        void set_view(math::matrix_float44 value)
		{
			m_view = value;
		}

        void set_light_count(uint32_t value)
        {
            m_light_count = value;
        }

        void set_light_position_ws( const math::vector_float4* begin, const math::vector_float4* end )
        {
            std::copy(begin, end, stdext::make_checked_array_iterator(&m_light_position_ws[0], 8, 0 ) );
        }

        void set_light_color( const math::vector_float4* begin, const math::vector_float4* end)
        {
            std::copy(begin, end, stdext::make_checked_array_iterator(&m_light_color[0], 8, 0) );
        }
*/

void directional_lights_entity::on_execute_draw_calls( gx::draw_call_context* context )
{
    auto device_context = context->m_device_context;

    std::get<1>(m_pixel_pipeline).set_inverse_projection(*context->m_inverse_projection_matrix);
    std::get<1>(m_pixel_pipeline).set_view(*context->m_view_matrix);

    std::get<1>(m_pixel_pipeline).set_light_count( static_cast<uint32_t> ( m_light_position_ws.size() ) );
    std::get<1>(m_pixel_pipeline).set_light_position_ws( &m_light_position_ws[0], &m_light_position_ws[0] + m_light_position_ws.size() );
    std::get<1>(m_pixel_pipeline).set_light_color( &m_light_color[0], &m_light_color[0] + m_light_color.size() );

    std::get<1>(m_pixel_pipeline).flush(device_context);
    std::get<1>(m_pixel_pipeline).bind_as_pixel_constant_buffer(device_context);

    dx11::vs_set_shader(device_context, std::get<0>(m_vertex_pipeline) );
	dx11::ps_set_shader(device_context, std::get<0>(m_pixel_pipeline) );
	device_context->IASetInputLayout( std::get<1>(m_vertex_pipeline) );

    uint32_t stride = 12;
    uint32_t offset = 0;

    ID3D11Buffer*	buffers[] = { m_quad_data.get() } ;

	device_context->IASetVertexBuffers( 0, 1, &buffers[0], &stride, &offset );

    device_context->Draw( 6, 0 );

}

std::shared_ptr<directional_lights_entity> create_directional_lights_entity(    ID3D11Device* device,  dx11::id3d11buffer_ptr quad_data, const directional_light* begin, const directional_light* end  )
{
    return std::make_shared<directional_lights_entity>( quad_data, gx::create_light_directional_vertex_pipeline(device), 
                                                        gx::create_light_directional_pixel_pipeline(device), begin, end );

}




