#ifndef __GX_DRAW_CALL_CONTEXT_H__
#define __GX_DRAW_CALL_CONTEXT_H__

#include <cstdint>
#include <math/math_matrix.h>

struct ID3D11DeviceContext;

namespace gx
{
    struct draw_call_context
    {
        ID3D11DeviceContext*			m_device_context;

		math::matrix_float44*           m_view_matrix;
        math::matrix_float44*           m_projection_matrix;
        math::matrix_float44*           m_inverse_view_matrix;
        math::matrix_float44*           m_inverse_projection_matrix;
        math::matrix_float44*           m_wvp_matrix;
		math::matrix_float44*           m_world_matrix;

		uint16_t						m_entity_draw_call_index;
    };
}

#endif