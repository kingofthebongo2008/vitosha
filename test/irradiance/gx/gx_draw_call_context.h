#ifndef __GX_DRAW_CALL_CONTEXT_H__
#define __GX_DRAW_CALL_CONTEXT_H__

#include <cstdint>
#include <math/math_matrix.h>
#include <dx11/dx11_pointers.h>

namespace gx
{
    class draw_call_context
    {
        public:

        dx11::id3d11devicecontext_ptr	m_device_context;

        math::matrix_float44*           m_view_matrix;
        math::matrix_float44*           m_projection_matrix;
        math::matrix_float44*           m_inverse_view_matrix;
        math::matrix_float44*           m_inverse_projection_matrix;
        math::matrix_float44*           m_pvw_matrix;
        math::matrix_float44*           m_inverse_pvw_matrix;
    };
}

#endif