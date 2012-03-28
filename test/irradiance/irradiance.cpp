#include "precompiled.h"


#include <dx11/dx11_system.h>

#include <fnd/fnd_universe.h>

#include <gx/gx_entity.h>
#include <gx/gx_color.h>
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

    math::matrix_float44 m = math::identity_matrix();

	const float pi_div_two		= 1.570796327f;
	math::matrix_float44 m_1 = math::rotation_x(pi_div_two);
    math::matrix_float44 m_2 = math::scaling(1.0f, 1.0f, 1.0f);

    std::shared_ptr<gx::scene::node> root = scene->get_root();

    std::shared_ptr< gx::entity > entity_1 = gxu::create_lat_lon_sphere_entity( render_context, 1.0f, 20, gx::color::blue() ); 
	std::shared_ptr<gx::scene::node> node_1 = std::make_shared<gx::scene::node> ( m, entity_1.get() );

	//1. add to graphic world
	gx::add_node(root, node_1);
	
	//2. add to entities world
	entities->add_entity( entity_1);

	scene->rebuild();
	
	return scene;
}

