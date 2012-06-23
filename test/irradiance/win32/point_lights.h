#ifndef __POINT_LIGHTS_H__
#define __POINT_LIGHTS_H__

#include <memory>
#include <vector>

#include <util/util_memory.h>

#include <math/math_vector.h>
#include <dx11/dx11_pointers.h>

#include <fnd/fnd_updater.h>

#include <gx/gx_entity.h>
#include <gx/gx_indexed_draw_call.h>

#include "point_light_sphere_debug_vertex_shader.h"
#include "point_light_sphere_debug_pixel_shader.h"


class __declspec(align(16)) point_light
{
	public:

	point_light( math::float4 power, math::float4 position, float radius, float cut_off_distance ) : m_power(power), m_position(position)
	{
		set_radius(radius);
		set_cut_off_distance(cut_off_distance);
	}

	math::float4 get_power() const
	{
		const uint32_t __declspec(align(16)) mask[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 };

		auto v = math::load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		return math::and( m_power, v );
	}

	math::float4 set_power(math::float4 value)
	{
		using namespace math;

		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};

		auto v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		//retain the cut_off_distance
		m_power = select(value, m_power, v_1);
	}

	math::float4 get_position() const
	{
		const uint32_t __declspec(align(16)) mask[4] = {0, 0, 0, 0xFFFFFFFF };
		static const math::float4 identity_r4  = {0.0f, 0.0f, 0.0f, 1.0f};

		auto v = math::load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		return math::select( m_position, identity_r4, v );
	}

	math::float4 set_position(math::float4 value)
	{
		using namespace math;

		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};

		auto v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		//retain the cut_off_distance
		m_position = select(value, m_position, v_1);
	}

	float get_radius() const
	{
		using namespace math;

		float __declspec(align(16)) result;

		auto v = swizzle<x,x,x,x>(m_power);

		store1(&result, v);

		return result;
	}

	void set_radius(float value)
	{
		using namespace math;

		float4 v = splat(value);

		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};

		auto v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		m_power = select(m_power, v, v_1);
	}

	float get_cut_off_distance() const
	{
		using namespace math;

		float __declspec(align(16)) result;

		auto v = swizzle<x,x,x,x>(m_position);

		store1(&result, v);

		return result;
	}

	void set_cut_off_distance(float value)
	{
		using namespace math;

		auto v = splat(value);

		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};

		auto v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		m_position = select(m_position, v, v_1);

	}

	private:

	math::float4 m_power;
	math::float4	m_position;

};

class point_lights_entity : public gx::entity
{
	public:

	static const uint32_t max_light_count = 1024;

	//workaround for visual studio 11 5 parameters to make_shared
	struct shader_info
	{
		shader_info
				(
					point_light_sphere_debug_vertex_shader					debug_vertex_shader,
					point_light_sphere_debug_pixel_shader					debug_pixel_shader,
					point_light_sphere_debug_vertex_shader_constant_buffer	cbuffer
				) :
						m_debug_vertex_shader(debug_vertex_shader)
					,	m_debug_pixel_shader(debug_pixel_shader)
					,	m_cbuffer(cbuffer)
				{

				}

		point_light_sphere_debug_vertex_shader					m_debug_vertex_shader;
		point_light_sphere_debug_pixel_shader					m_debug_pixel_shader;
		point_light_sphere_debug_vertex_shader_constant_buffer	m_cbuffer;

	};

	point_lights_entity
		( 
			gx::indexed_draw_call_3	draw_call,
			dx11::id3d11buffer_ptr	transform_color,
			dx11::id3d11inputlayout_ptr	input_layout,
			shader_info info
		);

	void add_light( point_light light)
	{
		m_lights.push_back(light);
	}

	void add_lights( const point_light* begin, const point_light* end);
	void update_light_positions( float dt);

	void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector);
	void on_execute_draw_calls(gx::draw_call_context* context);

	private:
	gx::indexed_draw_call_3	m_draw_call;		//sphere vertex and index buffer (holds reference to the instance stream inside)
	dx11::id3d11buffer_ptr	m_transform_color;	//instance stream
	dx11::id3d11inputlayout_ptr	m_input_layout;

	point_light_sphere_debug_vertex_shader					m_debug_vertex_shader;
	point_light_sphere_debug_pixel_shader					m_debug_pixel_shader;
	point_light_sphere_debug_vertex_shader_constant_buffer	m_cbuffer;

	std::vector< point_light >	m_lights;

	void update_instance_stream(ID3D11DeviceContext* device_context, math::float4x4 world_matrix);
};

std::shared_ptr<point_lights_entity> create_point_lights_entity(ID3D11Device* device);

struct update_point_lights
{
	void operator()(point_lights_entity* entity, float dt)
	{
		entity->update_light_positions(dt);
	}
};

typedef fnd::phase_two_updater<point_lights_entity, update_point_lights >  point_light_updater;

inline std::shared_ptr< point_light_updater > create_light_entity_updater()
{
	std::shared_ptr<point_light_updater> updater = std::make_shared< point_light_updater >  ( 0, update_point_lights() );
	return updater;
}





#endif
