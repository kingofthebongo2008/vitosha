#include "precompiled.h"

#include <fnd/fnd_universe.h>
#include <gx/gx_scene.h>

void universe_bootstrap( std::shared_ptr<fnd::universe> universe )
{
    std::shared_ptr<gx::scene> scene = std::make_shared<gx::scene>();

    universe->add_world(scene);

    auto it = scene->begin();
    auto it_end = scene->end();

    math::matrix_float44 m = math::matrix44_identity();
    math::matrix_float44 m_1 = math::matrix44_scaling(2.0f, 2.0f, 2.0f);
    math::matrix_float44 m_2 = math::matrix44_scaling(2.0f, 2.0f, 2.0f);

    std::shared_ptr<gx::scene::node> root = scene->get_root();

    std::shared_ptr<gx::scene::node> node_1 = std::make_shared<gx::scene::node> ( m_1, reinterpret_cast<void*> (0x01L) );
    std::shared_ptr<gx::scene::node> node_2 = std::make_shared<gx::scene::node> ( m_1, reinterpret_cast<void*> (0x02L) );
    std::shared_ptr<gx::scene::node> node_3 = std::make_shared<gx::scene::node> ( m_1, reinterpret_cast<void*> (0x03L) );
    std::shared_ptr<gx::scene::node> node_4 = std::make_shared<gx::scene::node> ( m_1, reinterpret_cast<void*> (0x04L) );

    std::shared_ptr<gx::scene::node> node_11 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x011L) );
    std::shared_ptr<gx::scene::node> node_12 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x012L) );
    std::shared_ptr<gx::scene::node> node_13 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x013L) );
    std::shared_ptr<gx::scene::node> node_14 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x014L) );

    std::shared_ptr<gx::scene::node> node_21 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x021L) );
    std::shared_ptr<gx::scene::node> node_22 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x022L) );
    std::shared_ptr<gx::scene::node> node_23 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x023L) );
    std::shared_ptr<gx::scene::node> node_24 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x024L) );

    std::shared_ptr<gx::scene::node> node_31 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x031L) );
    std::shared_ptr<gx::scene::node> node_32 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x032L) );
    std::shared_ptr<gx::scene::node> node_33 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x033L) );
    std::shared_ptr<gx::scene::node> node_34 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x034L) );

    std::shared_ptr<gx::scene::node> node_41 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x041L) );
    std::shared_ptr<gx::scene::node> node_42 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x042L) );
    std::shared_ptr<gx::scene::node> node_43 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x043L) );
    std::shared_ptr<gx::scene::node> node_44 = std::make_shared<gx::scene::node> ( m_2, reinterpret_cast<void*> (0x044L) );

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
}

