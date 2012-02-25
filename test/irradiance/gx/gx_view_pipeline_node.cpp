#include "precompiled.h"
#include <gx/gx_view_pipeline_node.h>
#include <gx/gx_pipeline_params.h>

#include <gx/gx_view.h>


namespace gx
{
    void*   view_pipeline_node::do_process(void* input)
    {
        scene_pipeline_params* params = reinterpret_cast<scene_pipeline_params*> (input);

        uint32_t size = static_cast<uint32_t> ( params->m_world_matrices->size() );

        m_pvw_matrices.resize( size );

        math::matrix_float44 view_matrix = m_view->get_view_matrix();
        math::matrix_float44 projection_matrix = m_view->get_projection_matrix();

        math::matrix_float44 inverse_view_matrix = math::matrix44_inverse(view_matrix);
        math::matrix_float44 inverse_projection_matrix = math::matrix44_inverse(projection_matrix);

        view_pipeline_params view_params = {
                                view_matrix, projection_matrix, 
                                inverse_view_matrix, inverse_projection_matrix,
                                &m_pvw_matrices, params->m_world_matrices, params->m_data } ;

        math::matrix_float44 pv = math::matrix44_mul( projection_matrix, view_matrix );
        
        for (uint32_t i = 0; i < size; ++i)
        {
            m_pvw_matrices[i] = math::matrix44_mul( pv, params->m_world_matrices->operator[](i) );
        }

        delete params;
        return new view_pipeline_params(view_params);
    }
}