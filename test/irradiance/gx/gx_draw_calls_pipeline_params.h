#ifndef __GX_DRAW_CALLS_PIPELINE_PARAMS_H__
#define __GX_DRAW_CALLS_PIPELINE_PARAMS_H__

#include <vector>

#include <math/math_matrix.h>
#include <gx/gx_draw_call_collector.h>

namespace gx
{
    struct draw_calls_pipeline_params
    {
        math::matrix_float44                    m_view_matrix;
        math::matrix_float44                    m_projection_matrix;
        math::matrix_float44                    m_inverse_view_matrix;
        math::matrix_float44                    m_inverse_projection_matrix;

        std::vector< math::matrix_float44 >*    m_pvw_matrices;
        std::vector< math::matrix_float44 >*    m_world_matrices;
        std::vector < draw_call_info >*         m_draw_calls;
        std::vector< void* >*                   m_data;
    };

}

#endif