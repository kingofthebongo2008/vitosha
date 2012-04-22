#include "precompiled.h"

#include <algorithm>

#include <dx11/dx11_system.h>
#include <math/math_graphics.h>

#include <fnd/fnd_universe.h>
#include <fnd/fnd_updater.h>

#include <gx/gx_color.h>
#include <gx/gx_entity.h>
#include <gx/gx_entity_world.h>
#include <gx/gx_lambert_shift_invariant_material.h>
#include <gx/gx_scene.h>

#include <gxu/gxu_entity_factory.h>

#include "win32/point_lights.h"


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
	auto point_lights = create_point_lights_entity( context.m_device.get() );
	auto light_updater = create_light_entity_updater();
	dynamic_entities->add_type( 0, light_updater );

	point_light light1 ( math::set(0.3f, 0.2f, 0.0f, 0.0f), math::set(1.5f,0.0f,0.0f,1.0f), 1.0f, 0.5f);
	point_light light2 ( math::set(0.5f, 0.2f, 0.5f, 0.0f), math::set(0.0f,1.0f,6.0f,1.0f), 1.0f, 0.5f);
	point_light light3 ( math::set(0.5f, 0.2f, 0.5f, 0.0f), math::set(-1.0f,1.0f,6.0f,1.0f), 1.0f, 0.5f);

	point_lights->add_light(light1);
	point_lights->add_light(light2);
	point_lights->add_light(light3);

    auto it = scene->begin();
    auto it_end = scene->end();

	auto pi_div_two		= 1.570796327f;

	auto m_1 = math::rotation_x(-pi_div_two);
	auto m_2 = math::translation(-1.0f, 0.0f, 0.0f);
	auto m_3 = math::mul(m_1, m_2);
	auto m_4 = math::translation(0.0f, 0.0f, 0.0f);
	
	auto root = scene->get_root();

    auto entity_1 = gxu::create_lat_lon_sphere_entity<gx::lambert_shift_invairant_material_factory>( render_context, 1.0f, 20, gx::color::blue() ); 
	
	auto node_1 = std::make_shared<gx::scene::node> ( m_3, entity_1.get() );
	auto node_2 = std::make_shared<gx::scene::node> ( m_4, point_lights.get() );

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

