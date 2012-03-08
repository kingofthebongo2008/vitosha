#ifndef __GX_PIPELINE_PARAMS_H__
#define __GX_PIPELINE_PARAMS_H__

#include <vector>

#include <math/math_matrix.h>

namespace gx
{
    struct scene_pipeline_params
    {
        std::vector< math::matrix_float44 >* m_world_matrices;
        std::vector< void* >*                m_data;
    };

    struct view_pipeline_params
    {
        math::matrix_float44                 m_view_matrix;
        math::matrix_float44                 m_projection_matrix;
        math::matrix_float44                 m_inverse_view_matrix;
        math::matrix_float44                 m_inverse_projection_matrix;
        std::vector< math::matrix_float44 >* m_wvp_matrices;
        std::vector< math::matrix_float44 >* m_world_matrices;
        std::vector< void* >*                m_data;
    };
}

#endif