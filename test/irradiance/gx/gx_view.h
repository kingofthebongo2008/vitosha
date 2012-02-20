#ifndef __GX_VIEW_H__
#define __GX_VIEW_H__

#include <cstdint>
#include <math/math_matrix.h>

namespace gx
{
    class view
    {
        public:
        view() : m_view( math::matrix44_identity()), m_projection(math::matrix44_identity() )
        {
        }

        view(math::matrix_float44 view, math::matrix_float44 projection) : m_view(view), m_projection(projection)
        {

        }

        private:

        math::matrix_float44  m_view;
        math::matrix_float44  m_projection;
    };
}

#endif