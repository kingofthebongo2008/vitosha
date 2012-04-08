#include "precompiled.h"
#include <gx/gx_scene_pipeline_node.h>
#include <gx/gx_scene.h>

#include <gx/gx_pipeline_params.h>

#include <iterator>

namespace gx
{

    void* scene_pipeline_node::do_process(void*)
    {
        m_scene->build();
        m_scene->process(this);
        scene_pipeline_params params = { &m_world_matrices, &m_data };
        return new scene_pipeline_params (params); 
    }

    
    void scene_pipeline_node::submit( const std::vector< math::matrix_float44>* world_matrices, const std::vector<void*>* data )
    {
        m_world_matrices.reserve( world_matrices->size() );
        m_data.reserve( data->size() );

        m_world_matrices.clear();
        m_data.clear();

        //skip the root node
        std::copy( std::begin(*world_matrices) + 1 , std::end(*world_matrices), std::back_inserter< std::vector< math::matrix_float44>> (m_world_matrices) );
        std::copy( std::begin(*data) + 1 , std::end(*data), std::back_inserter< std::vector<void*>> (m_data) );
    }
}