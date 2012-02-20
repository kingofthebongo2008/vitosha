#include "precompiled.h"
#include <gx/gx_scene.h>

namespace gx
{
    scene::scene() : 
        m_end ( &m_end)
        , m_cend( &m_cend)
        , m_status(clean)
    {

        m_world_matrices.reserve(4096);
        m_matrices.reserve(4096);
        m_data.reserve(4096);
        m_flat_nodes.reserve(4096);

        m_root = std::make_shared<node>( );
        rebuild();
    }

    void scene::do_build()
    {
        uint32_t size = static_cast<uint32_t> ( m_matrices.size() );
        m_world_matrices.resize(size);

        for ( uint32_t i = 0; i  < size; ++i)
        {
            flat_node_parent_info info = m_flat_nodes[i];
            m_world_matrices[i] = math::matrix44_mul( m_matrices[info], m_matrices[i]);
        }
    }

    void scene::rebuild()
    {
        m_world_matrices.clear();
        m_matrices.clear();
        m_data.clear();
        m_flat_nodes.clear();

        std::vector<std::shared_ptr<node> >                                         stack_nodes_vector;
        std::vector<uint32_t >                                                      stack_indices_vector;

        stack_nodes_vector.reserve(128);
        stack_indices_vector.reserve(128);
        
        typedef std::stack< std::shared_ptr<node>, std::vector<std::shared_ptr<node> > > stack_container_t;
        typedef std::stack< uint32_t, std::vector<uint32_t> >                            stack_indices_t;

        stack_container_t   stack_nodes(stack_nodes_vector);
        stack_indices_t     stack_indices(stack_indices_vector);

        stack_nodes.push(get_root());
        stack_indices.push(0);

        while (!stack_nodes.empty())
        {
            std::shared_ptr<node> node = stack_nodes.top();
            uint32_t              parent_index = stack_indices.top();
            
            stack_nodes.pop();
            stack_indices.pop();

            m_flat_nodes.push_back( parent_index );
            m_data.push_back( node->get_data() );
            m_matrices.push_back( node->get_transform() );

            uint32_t index = static_cast<uint32_t> ( m_flat_nodes.size() - 1) ;
            node->set_node_transform( &m_matrices[index] );

            if ( !node->m_children.empty() )
            {
                std::copy(node->m_children.rbegin(), node->m_children.rend(), util::push_iterator<stack_container_t> (stack_nodes) );
                std::fill_n( util::push_iterator<stack_indices_t> (stack_indices), node->m_children.size(), index );
            }
        }

        set_status(modified);
        build();
    }
}