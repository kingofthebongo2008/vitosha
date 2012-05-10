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

        view(math::float4x4 view, math::float4x4 projection) : m_view_matrix(view), m_projection_matrix(projection)
        {

        }

        math::float4x4 get_view_matrix() const
        {
            return m_view_matrix;
        }

        math::float4x4 get_projection_matrix() const
        {
            return m_projection_matrix;
        }

        private:

        math::float4x4  m_view_matrix;
        math::float4x4  m_projection_matrix;
    };
}

#endif