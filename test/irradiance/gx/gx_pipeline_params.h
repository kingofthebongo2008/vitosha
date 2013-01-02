#ifndef __GX_PIPELINE_PARAMS_H__
#define __GX_PIPELINE_PARAMS_H__

#include <vector>

#include <math/math_matrix.h>

#include <gx/gx_view_port.h>

namespace gx
{
    struct scene_pipeline_params
    {
        std::vector< math::float4x4 >* m_world_matrices;
        std::vector< void* >*          m_data;
    };

    struct view_pipeline_params
    {
        math::float4x4                 m_view_matrix;
        math::float4x4                 m_projection_matrix;
        math::float4x4                 m_inverse_view_matrix;
        math::float4x4                 m_inverse_projection_matrix;

		view_port					   m_view_port;
		float						   m_zn;
		float						   m_zf;

        std::vector< math::float4x4 >* m_wvp_matrices;
        std::vector< math::float4x4 >* m_world_matrices;
        std::vector< void* >*          m_data;
    };
}

#endif