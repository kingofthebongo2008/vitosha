#ifndef __RENDER_VIEW_PORT_H__
#define __RENDER_VIEW_PORT_H__

#include <cstdint>

#include "math/math_graphics.h"

namespace gx
{
    class view_port
    {
        public:
        view_port() : m_left(0), m_top(0), m_width(320), m_height(240), m_min_z(0.0f), m_max_z(1.0f)
        {

        }

		view_port( uint32_t left, uint32_t top, uint32_t width, uint32_t height, float min_z, float max_z) :
			m_left(static_cast<uint16_t> (left) )
			, m_top(static_cast<uint16_t> (top) )
			, m_width(static_cast<uint16_t> (width) ) 
			, m_height(static_cast<uint16_t> (height) ) 
			, m_min_z(min_z) 
			, m_max_z(max_z)
		{

		}

		view_port( uint32_t left, uint32_t top, uint32_t width, uint32_t height) :
			m_left(static_cast<uint16_t> (left) )
			, m_top(static_cast<uint16_t> (top) )
			, m_width(static_cast<uint16_t> (width) ) 
			, m_height(static_cast<uint16_t> (height) ) 
			, m_min_z(0.0f) 
			, m_max_z(1.0f)
		{

		}

        inline uint32_t get_height() const
        {
            return m_height;
        }

        inline uint32_t get_width() const
        {
            return m_width;
        }

        inline uint32_t get_left() const
        {
            return m_left;
        }

        inline uint32_t get_top() const
        {
            return m_top;
        }

		inline float get_min_z() const
		{
			return m_min_z;
		}

		inline float get_max_z() const
		{
			return m_max_z;
		}

        inline void set_dimensions(uint32_t width, uint32_t height)
        {
            m_width = static_cast<uint16_t>(width);
            m_height = static_cast<uint16_t>(height);
        }

        inline void set_offset(uint32_t left, uint32_t top)
        {
            m_left = static_cast<uint16_t>(left);
            m_top = static_cast<uint16_t>(top);
        }

        inline void set_depth(float min_z, float max_z)
        {
            m_min_z = min_z;
            m_max_z = max_z;
        }

		operator math::view_port()  const
		{
			const math::view_port v =
							{ 
									static_cast<float> ( m_left ), 
									static_cast<float> ( m_top ),
									static_cast<float> ( m_left	+ m_width ),
									static_cast<float> ( m_top	+ m_height ),
														 m_min_z, 
														 m_max_z
							};
			return v;
		}
        
        private:

        float   m_min_z;
        float   m_max_z;

        uint16_t m_left;
        uint16_t m_top;

        uint16_t m_width;
        uint16_t m_height;
    };
}

#endif