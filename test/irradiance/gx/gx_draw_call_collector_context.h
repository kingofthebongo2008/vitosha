#ifndef __GX_DRAW_CALL_COLLECTOR_CONTEXT_H__
#define __GX_DRAW_CALL_COLLECTOR_CONTEXT_H__

#include <cstdint>

#include <math/math_matrix.h>

namespace gx
{
   struct draw_call_collector_context
   {
        math::matrix_float44*    m_view_matrix;
        math::matrix_float44*    m_projection_matrix;

        math::matrix_float44*    m_world_matrix;
        math::matrix_float44*    m_wvp;
		uint32_t			 m_entity_index;
   };

}

#endif