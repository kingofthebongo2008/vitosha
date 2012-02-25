#ifndef __GX_DRAW_CALL_COLLECTOR_CONTEXT_H__
#define __GX_DRAW_CALL_COLLECTOR_CONTEXT_H__

#include <cstdint>

#include <math/math_matrix.h>

namespace gx
{
   class draw_call_collector_context
   {
        public:
        math::matrix_float44*    m_view_matrix;
        math::matrix_float44*    m_projection_matrix;

        math::matrix_float44*    m_world_matrix;
        math::matrix_float44*    m_pvw;
   };

}

#endif