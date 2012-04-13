#include "precompiled.h"

#include <algorithm>

#include <dx11/dx11_system.h>
#include <math/math_graphics.h>

#include <fnd/fnd_universe.h>
#include <fnd/fnd_updater.h>

#include <gx/gx_color.h>
#include <gx/gx_entity.h>
#include <gx/gx_entity_world.h>
#include <gx/gx_lambert_constant_material.h>
#include <gx/gx_scene.h>

#include <gxu/gxu_entity_factory.h>

class __declspec(align(16)) point_light
{
	point_light( math::vector_float4 power, math::vector_float4 position, float radius, float cut_off_distance ) : m_power(power), m_position(position)
	{
		set_radius(radius);
		set_cut_off_distance(cut_off_distance);
	}

	math::vector_float4 get_power() const
	{
		const uint32_t __declspec(align(16)) mask[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 };

		auto v = math::load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		return math::and( m_power, v );
	}

	math::vector_float4 set_power(math::vector_float4 value)
	{
		using namespace math;

		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};

		auto v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		//retain the cut_off_distance
		m_power = select(value, m_power, v_1);
	}

	math::vector_float4 get_position() const
	{
		const uint32_t __declspec(align(16)) mask[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 };

		auto v = math::load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		return math::and( m_position, v );
	}

	math::vector_float4 set_position(math::vector_float4 value)
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

		vector_float4 v = splat(value);

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

		m_position = select(m_power, v, v_1);

	}

	private:

	math::vector_float4 m_power;
	math::vector_float4	m_position;

};

class point_lights_entity : public gx::entity
{
	public:

	point_lights_entity ()
	{
		for (auto i = 0 ; i < 4; ++i)
		{
			m_lights[i].reserve(1024);
		}
	}

	void add_light( point_light light)
	{
		for (auto i = 0 ; i < 4; ++i)
		{
			m_lights[i].push_back(light);
		}
	}

	void add_lights( const point_light* begin, const point_light* end)
	{
		for (auto i = 0 ; i < 4; ++i)
		{
			m_lights[i].reserve ( end - begin );
			std::copy( begin, end, std::back_inserter(m_lights[i]) );
		}
	}

	void update_light_positions( float dt)
	{
		dt;
	}


	void on_create_draw_calls( gx::draw_call_collector_context* context, gx::draw_call_collector* collector)
	{
		uint16_t material_id = 2;
		gx::draw_call_key key = gx::create_debug_layer_key(material_id, gx::get_perspective_transform3_depth(context) );

		collector->add_draw_call(key, context->m_entity_index, 0 ) ;
		collector->add_draw_call(key, context->m_entity_index, 1 ) ;
    }

	void on_execute_draw_calls(gx::draw_call_context* context)
	{
		static int k=10;;
		k++;
	}

	private:

	std::vector< point_light >	m_lights[4];
};

struct update_point_lights
{
	void operator()(point_lights_entity* entity, float dt)
	{
		entity->update_light_positions(dt);
	}
};

void test(point_lights_entity* entity, float dt)
{

}

typedef fnd::phase_two_updater<point_lights_entity, update_point_lights >  point_light_updater;

std::shared_ptr< point_light_updater > create_light_entity_updater()
{
	std::shared_ptr<point_light_updater> updater = std::make_shared< point_light_updater >  ( 0, update_point_lights() );
	return updater;
}

std::shared_ptr<gx::scene> universe_bootstrap( gx::render_context* render_context, dx11::system_context context, std::shared_ptr<fnd::universe> universe )
{

    auto scene = std::make_shared<gx::scene>();
    auto entities = std::make_shared<gx::entity_world> ();
	auto dynamic_entities = std::make_shared<fnd::typed_world> ();

	//add two worlds
	//1. with entities, created or loaded
	//2. dynamic entities which change depending on time
	//3. add a spatial structure for representing these entities
	universe->add_world(entities);
	universe->add_world(dynamic_entities);
	universe->add_world(scene);


	//create point light entity and insert it into two worlds (for update and render)
	auto point_lights = std::make_shared<point_lights_entity>();
	auto light_updater = create_light_entity_updater();
	dynamic_entities->add_type( 0, light_updater );


    auto it = scene->begin();
    auto it_end = scene->end();

	auto pi_div_two		= 1.570796327f;

	auto m_1 = math::rotation_x(-pi_div_two);
	auto m_2 = math::translation(0.0f, 0.0f, 0.0f);
	auto m_3 = math::mul(m_1, m_2);
	
	auto root = scene->get_root();

    auto entity_1 = gxu::create_lat_lon_sphere_entity<gx::lambert_constant_material_factory>( render_context, 1.0f, 20, gx::color::blue() ); 
	
	auto node_1 = std::make_shared<gx::scene::node> ( m_3, entity_1.get() );
	auto node_2 = std::make_shared<gx::scene::node> ( math::identity_matrix(), point_lights.get() );

	//1. add to graphic world
	gx::add_node(root, node_1);
	gx::add_node(root, node_2);
	
	//2. add to entities world
	entities->add_entity( entity_1);
	entities->add_entity( point_lights);

	scene->rebuild();

	//3. add lights for updating
	light_updater->add_item(point_lights);
	
	return scene;
}

