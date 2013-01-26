#ifndef __DIRECTIONAL_LIGHTS_H__
#define __DIRECTIONAL_LIGHTS_H__

#include <memory>
#include <vector>

#include <math/math_vector.h>
#include <d3d11/d3d11_pointers.h>

#include <gx/gx_entity.h>

#include <gx/gx_light_directional_pixel_shader.h>
#include <gx/gx_light_directional_vertex_shader.h>


class __declspec(align(16)) directional_light
{
	public:

	directional_light( math::float4 power, math::float4 direction_ws ) : m_power(power), m_direction_ws(direction_ws)
	{

    }

    math::float4 get_power() const
	{
        return m_power;
	}

	math::float4 set_power(math::float4 value)
	{
		m_power = value;
	}

	math::float4 get_direction() const
	{
        return m_direction_ws;
	}

	private:

	math::float4	m_power;
	math::float4	m_direction_ws;

};

class directional_lights_entity : public gx::entity
{
	public:

    directional_lights_entity
    ( 
        d3d11::ibuffer_ptr						quad_data,

        gx::light_directional_vertex_pipeline   vertex_pipeline,
        gx::light_directional_pixel_pipeline    pixel_pipeline,

        const directional_light* begin,
        const directional_light* end
	);

    void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector);
	void on_execute_draw_calls( gx::draw_call_context* context );

    private:

    d3d11::ibuffer_ptr						m_quad_data;

    gx::light_directional_vertex_pipeline   m_vertex_pipeline;
    gx::light_directional_pixel_pipeline    m_pixel_pipeline;

    std::vector<math::float4>				m_light_direction_ws;
    std::vector<math::float4>				m_light_color;
};

std::shared_ptr<directional_lights_entity> create_directional_lights_entity(    ID3D11Device* device,  d3d11::ibuffer_ptr quad_data, const directional_light* begin, const directional_light* end  );


#endif
