#include "precompiled.h"


#include <dx11/dx11_system.h>

#include <fnd/fnd_universe.h>

#include <gx/gx_entity.h>
#include <gx/gx_scene.h>

#include <gxu/gxu_entity_factory.h>

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

std::shared_ptr<gx::scene> universe_bootstrap( dx11::system_context context, std::shared_ptr<fnd::universe> universe )
{
    std::shared_ptr<gx::scene> scene = std::make_shared<gx::scene>();
    std::shared_ptr<entity_world> entities = std::make_shared<entity_world> ();

    universe->add_world(entities);
    universe->add_world(scene);

    auto it = scene->begin();
    auto it_end = scene->end();

    math::matrix_float44 m = math::matrix44_identity();
    math::matrix_float44 m_1 = math::matrix44_scaling(2.0f, 2.0f, 2.0f);
    math::matrix_float44 m_2 = math::matrix44_scaling(2.0f, 2.0f, 2.0f);

    std::shared_ptr<gx::scene::node> root = scene->get_root();

    std::shared_ptr< gx::entity > entity_1 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_2 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_3 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_4 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 

    std::shared_ptr< gx::entity > entity_11 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_12 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_13 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_14 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 

    std::shared_ptr< gx::entity > entity_21 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_22 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_23 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_24 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    
    std::shared_ptr< gx::entity > entity_31 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_32 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_33 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_34 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 

    std::shared_ptr< gx::entity > entity_41 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_42 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_43 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 
    std::shared_ptr< gx::entity > entity_44 = gxu::create_lat_lon_sphere_entity( context.m_device.get(), 1.0f, 5 ); 

    entities->add_entity(entity_1);
    entities->add_entity(entity_2);
    entities->add_entity(entity_3);
    entities->add_entity(entity_4);

    entities->add_entity(entity_11);
    entities->add_entity(entity_12);
    entities->add_entity(entity_13);
    entities->add_entity(entity_14);

    entities->add_entity(entity_21);
    entities->add_entity(entity_22);
    entities->add_entity(entity_23);
    entities->add_entity(entity_24);

    entities->add_entity(entity_31);
    entities->add_entity(entity_32);
    entities->add_entity(entity_33);
    entities->add_entity(entity_34);

    entities->add_entity(entity_41);
    entities->add_entity(entity_42);
    entities->add_entity(entity_43);
    entities->add_entity(entity_44);

    std::shared_ptr<gx::scene::node> node_1 = std::make_shared<gx::scene::node> ( m_1, entity_1.get() );
    std::shared_ptr<gx::scene::node> node_2 = std::make_shared<gx::scene::node> ( m_1, entity_2.get() );
    std::shared_ptr<gx::scene::node> node_3 = std::make_shared<gx::scene::node> ( m_1, entity_3.get() );
    std::shared_ptr<gx::scene::node> node_4 = std::make_shared<gx::scene::node> ( m_1, entity_4.get() );

    std::shared_ptr<gx::scene::node> node_11 = std::make_shared<gx::scene::node> ( m_2, entity_11.get() );
    std::shared_ptr<gx::scene::node> node_12 = std::make_shared<gx::scene::node> ( m_2, entity_12.get() );
    std::shared_ptr<gx::scene::node> node_13 = std::make_shared<gx::scene::node> ( m_2, entity_13.get() );
    std::shared_ptr<gx::scene::node> node_14 = std::make_shared<gx::scene::node> ( m_2, entity_14.get() );

    std::shared_ptr<gx::scene::node> node_21 = std::make_shared<gx::scene::node> ( m_2, entity_21.get() );
    std::shared_ptr<gx::scene::node> node_22 = std::make_shared<gx::scene::node> ( m_2, entity_22.get() );
    std::shared_ptr<gx::scene::node> node_23 = std::make_shared<gx::scene::node> ( m_2, entity_23.get() );
    std::shared_ptr<gx::scene::node> node_24 = std::make_shared<gx::scene::node> ( m_2, entity_24.get() );

    std::shared_ptr<gx::scene::node> node_31 = std::make_shared<gx::scene::node> ( m_2, entity_31.get() );
    std::shared_ptr<gx::scene::node> node_32 = std::make_shared<gx::scene::node> ( m_2, entity_32.get() );
    std::shared_ptr<gx::scene::node> node_33 = std::make_shared<gx::scene::node> ( m_2, entity_33.get() );
    std::shared_ptr<gx::scene::node> node_34 = std::make_shared<gx::scene::node> ( m_2, entity_34.get() );

    std::shared_ptr<gx::scene::node> node_41 = std::make_shared<gx::scene::node> ( m_2, entity_41.get() );
    std::shared_ptr<gx::scene::node> node_42 = std::make_shared<gx::scene::node> ( m_2, entity_42.get() );
    std::shared_ptr<gx::scene::node> node_43 = std::make_shared<gx::scene::node> ( m_2, entity_43.get() );
    std::shared_ptr<gx::scene::node> node_44 = std::make_shared<gx::scene::node> ( m_2, entity_44.get() );

    gx::add_node(root, node_1);
    gx::add_node(root, node_2);
    gx::add_node(root, node_3);
    gx::add_node(root, node_4);

    gx::add_node(node_1, node_11);
    gx::add_node(node_1, node_12);
    gx::add_node(node_1, node_13);
    gx::add_node(node_1, node_14);

    gx::add_node(node_2, node_21);
    gx::add_node(node_2, node_22);
    gx::add_node(node_2, node_23);
    gx::add_node(node_2, node_24);

    gx::add_node(node_3, node_31);
    gx::add_node(node_3, node_32);
    gx::add_node(node_3, node_33);
    gx::add_node(node_3, node_34);

    gx::add_node(node_4, node_41);
    gx::add_node(node_4, node_42);
    gx::add_node(node_4, node_43);
    gx::add_node(node_4, node_44);

    scene->rebuild();

    return scene;
}

