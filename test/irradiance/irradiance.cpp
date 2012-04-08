#include "precompiled.h"


#include <dx11/dx11_system.h>

#include <fnd/fnd_universe.h>

#include <gx/gx_entity.h>
#include <gx/gx_color.h>
#include <gx/gx_lambert_constant_material.h>
#include <gx/gx_scene.h>

#include <gxu/gxu_entity_factory.h>

#include <math/math_graphics.h>

//loading system inserts in this world
class entity_world : public fnd::world
{
    public:

    entity_world()
    {
        m_entities.reserve(4096);
    }

    void add_entity( std::shared_ptr< gx::entity> entity)
    {
        m_entities.push_back(entity);
    }

    std::vector < std::shared_ptr< gx::entity> > m_entities;
};

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
		math::vector_float4 v = math::load4( reinterpret_cast<const float* __restrict> (&mask[0]) );
		return math::and( m_power, v );
	}

	math::vector_float4 set_power(math::vector_float4 value)
	{
		using namespace math;
		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};
		math::vector_float4 v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		//retain the cut_off_distance
		m_power = select(value, m_power, v_1);
	}

	math::vector_float4 get_position() const
	{
		const uint32_t __declspec(align(16)) mask[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 };
		math::vector_float4 v = math::load4( reinterpret_cast<const float* __restrict> (&mask[0]) );
		return math::and( m_position, v );
	}

	math::vector_float4 set_position(math::vector_float4 value)
	{
		using namespace math;
		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};
		math::vector_float4 v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );

		//retain the cut_off_distance
		m_position = select(value, m_position, v_1);
	}

	float get_radius() const
	{
		using namespace math;
		float __declspec(align(16)) result;
		vector_float4 v = swizzle<x,x,x,x>(m_power);
		store1(&result, v);
		return result;
	}

	void set_radius(float value)
	{
		using namespace math;
		vector_float4 v = splat(value);
		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};
		math::vector_float4 v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );
		m_power = select(m_power, v, v_1);
	}

	float get_cut_off_distance() const
	{
		using namespace math;

		float __declspec(align(16)) result;
		vector_float4 v = swizzle<x,x,x,x>(m_position);
		store1(&result, v);
		return result;
	}

	void set_cut_off_distance(float value)
	{
		using namespace math;
		vector_float4 v = splat(value);
		const uint32_t __declspec(align(16)) mask[4] = {0,0,0,0xFFFFFFFF};
		math::vector_float4 v_1 = load4( reinterpret_cast<const float* __restrict> (&mask[0]) );
		m_position = select(m_power, v, v_1);
	}

	private:

	math::vector_float4 m_power;
	math::vector_float4	m_position;

};

class point_light_world : public fnd::world
{
	public:
	point_light_world ()
	{
		for (uint32_t i = 0 ; i < 4; ++i)
		{
			m_lights[i].reserve(1024);
		}

		m_buffered_lights = &m_lights[0];
		m_buffer_counter = 0;
	}

	void add_light( point_light light)
	{
		for (uint32_t i = 0 ; i < 4; ++i)
		{
			m_lights[i].push_back(light);
		}
	}

	std::vector< point_light>** get_lights()
	{
		return &m_buffered_lights;
	}

	private:

	std::vector< point_light>*	m_buffered_lights;
	std::vector< point_light>	m_lights[4];		//holds up to 4 frames of animated lights
	uint32_t					m_buffer_counter;

	virtual void on_update_pass_one(float dt)
	{
		dt;
	}

	virtual void on_update_pass_two(float)
	{
		++m_buffer_counter;
		m_buffer_counter %= 4;
		m_buffered_lights = &m_lights[m_buffer_counter];
	}
};



class point_light_world_entity : public gx::entity
{
	public:
	point_light_world_entity( std::vector< point_light>**	point_lights) : m_point_lights(point_lights)
	{

	}

	private:
	std::vector< point_light>**	m_point_lights;
};


std::shared_ptr<gx::scene> universe_bootstrap( gx::render_context* render_context, dx11::system_context context, std::shared_ptr<fnd::universe> universe )
{
    std::shared_ptr<gx::scene> scene = std::make_shared<gx::scene>();
    std::shared_ptr<entity_world> entities = std::make_shared<entity_world> ();

	//add two worlds
	//1. with entities, created ot loaded
	//2. visual representation for these entitites
	universe->add_world(entities);

	universe->add_world(scene);

    auto it = scene->begin();
    auto it_end = scene->end();

    //math::matrix_float44 m = math::identity_matrix();

	const float pi_div_two		= 1.570796327f;
	math::matrix_float44 m_1 = math::rotation_x(-pi_div_two);
	math::matrix_float44 m_2 = math::translation(0.0f, 0.0f, 0.0f);

	math::matrix_float44 m_3 = math::mul(m_1, m_2);

    std::shared_ptr<gx::scene::node> root = scene->get_root();

    std::shared_ptr< gx::entity > entity_1 = gxu::create_lat_lon_sphere_entity<gx::lambert_constant_material_factory>( render_context, 1.0f, 20, gx::color::blue() ); 
	std::shared_ptr<gx::scene::node> node_1 = std::make_shared<gx::scene::node> ( m_3, entity_1.get() );

	//1. add to graphic world
	gx::add_node(root, node_1);
	
	//2. add to entities world
	entities->add_entity( entity_1);

	scene->rebuild();
	
	return scene;
}

