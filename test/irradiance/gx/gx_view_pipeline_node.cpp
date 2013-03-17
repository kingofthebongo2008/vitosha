#include "precompiled.h"
#include <gx/gx_view_pipeline_node.h>
#include <gx/gx_pipeline_params.h>

#include <gx/gx_view.h>


namespace gx
{
    void*   view_pipeline_node::do_process(void* input)
    {
        auto in_params = reinterpret_cast<scene_pipeline_params*> (input);

        auto size = static_cast<uint32_t> ( in_params->m_world_matrices->size() );

        m_wvp_matrices.resize( size );

        auto view_matrix = m_view->get_view_matrix();
        auto projection_matrix = m_view->get_projection_matrix();

        auto inverse_view_matrix = math::inverse(view_matrix);
        auto inverse_projection_matrix = math::inverse(projection_matrix);


        view_pipeline_params view_params = {
                                view_matrix, 
                                projection_matrix, 
                                inverse_view_matrix, 
                                inverse_projection_matrix,
                                m_view->get_view_port(),
                                m_view->get_zn(),
                                m_view->get_zf(),
                                &m_wvp_matrices, 
                                in_params->m_world_matrices, 
                                in_params->m_data } ;

        math::float4x4 vp = math::mul( view_matrix, projection_matrix  );

        for (auto i = static_cast<uint32_t> (0); i < size; ++i)
        {
            m_wvp_matrices[i] = math::mul( in_params->m_world_matrices->at(i) , vp  );
        }

        delete in_params;
        return new view_pipeline_params(view_params);
    }
}