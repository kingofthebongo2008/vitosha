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

}

void directional_lights_entity::on_execute_draw_calls( gx::draw_call_context* context )
{

}

std::shared_ptr<directional_lights_entity> create_directional_lights_entity(    ID3D11Device* device,  dx11::id3d11buffer_ptr quad_data, const directional_light* begin, const directional_light* end  )
{
    return std::make_shared<directional_lights_entity>( quad_data, gx::create_light_directional_vertex_pipeline(device), 
                                                        gx::create_light_directional_pixel_pipeline(device), begin, end );

}




