#ifndef __GX_VIEW_H__
#define __GX_VIEW_H__

#include <cstdint>
#include <math/math_matrix.h>
#include <gx/gx_view_port.h>

namespace gx
{
    class view
    {
        public:

        view() : m_view_matrix( math::identity_matrix()), m_projection_matrix(math::identity_matrix() ), m_zn(1.0f), m_zf(2.0f)
        {

        }

        view(math::float4x4 view, math::float4x4 projection, view_port view_port, float zf, float zn) : 
		  m_view_matrix(view) 
		, m_projection_matrix(projection)
		, m_view_port(view_port)
		, m_zf(zf)
		, m_zn(zn)
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

		view_port get_view_port() const
		{
			return m_view_port;
		}

		float get_zn() const
		{
			return m_zn;
		}

		float get_zf() const
		{
			return m_zf;
		}

        private:

        math::float4x4  m_view_matrix;
        math::float4x4  m_projection_matrix;
		view_port		m_view_port;
		float			m_zn;
		float			m_zf;
    };
}

#endif