#include "precompiled.h"
#include <gx/gx_view_pipeline_node.h>
#include <gx/gx_pipeline_params.h>

#include <gx/gx_view.h>


namespace gx
{
    void*   view_pipeline_node::do_process(void* input)
    {
        scene_pipeline_params* params = reinterpret_cast<scene_pipeline_params*> (input);

        uint32_t size = params->m_world_matrices->size();
        m_wvp_matrices.resize( size );

        math::matrix_float44 view_matrix = m_view->get_view_matrix();
        math::matrix_float44 projection_matrix = m_view->get_projection_matrix();

        view_pipeline_params view_params = {view_matrix, projection_matrix, &m_wvp_matrices, params->m_world_matrices, params->m_data } ;

        math::matrix_float44 vp = math::matrix44_mul( view_matrix, projection_matrix );
        
        for (uint32_t i = 0; i < size; ++i)
        {
            m_wvp_matrices[i] = math::matrix44_mul( params->m_world_matrices->operator[](i), vp );
        }

        delete params;
        return new view_pipeline_params(view_params);
    }
}