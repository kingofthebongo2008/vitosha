#ifndef __GX_VIEW_H__
#define __GX_VIEW_H__

#include <cstdint>
#include <math/math_matrix.h>

namespace gx
{
    class view
    {
        public:

        view() : m_view_matrix( math::identity_matrix()), m_projection_matrix(math::identity_matrix() )
        {

        }

        view(math::matrix_float44 view, math::matrix_float44 projection) : m_view_matrix(view), m_projection_matrix(projection)
        {

        }

        math::matrix_float44 get_view_matrix() const
        {
            return m_view_matrix;
        }

        math::matrix_float44 get_projection_matrix() const
        {
            return m_projection_matrix;
        }

        private:

        math::matrix_float44  m_view_matrix;
        math::matrix_float44  m_projection_matrix;
    };
}

#endif