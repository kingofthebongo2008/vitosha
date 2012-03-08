#ifndef __RENDER_VIEW_PORT_H__
#define __RENDER_VIEW_PORT_H__

#include <cstdint>

namespace gx
{
    class view_port
    {
        public:
        view_port() : m_left(0), m_top(0), m_width(320), m_height(240), m_min_z(0.0f), m_max_z(1.0f)
        {

        }

		view_port( std::uint32_t left, std::uint32_t top, std::uint32_t width, std::uint32_t height, float min_z, float max_z) :
			m_left(static_cast<std::uint16_t> (left) )
			, m_top(static_cast<std::uint16_t> (top) )
			, m_width(static_cast<std::uint16_t> (width) ) 
			, m_height(static_cast<std::uint16_t> (height) ) 
			, m_min_z(min_z) 
			, m_max_z(max_z)
		{

		}

		view_port( std::uint32_t left, std::uint32_t top, std::uint32_t width, std::uint32_t height) :
			m_left(static_cast<std::uint16_t> (left) )
			, m_top(static_cast<std::uint16_t> (top) )
			, m_width(static_cast<std::uint16_t> (width) ) 
			, m_height(static_cast<std::uint16_t> (height) ) 
			, m_min_z(0.0f) 
			, m_max_z(1.0f)
		{

		}

        inline std::uint32_t get_height() const
        {
            return m_height;
        }

        inline std::uint32_t get_width() const
        {
            return m_width;
        }

        inline std::uint32_t get_left() const
        {
            return m_left;
        }

        inline std::uint32_t get_top() const
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

        inline void set_dimensions(std::uint32_t width, std::uint32_t height)
        {
            m_width = static_cast<std::uint16_t>(width);
            m_height = static_cast<std::uint16_t>(height);
        }

        inline void set_offset(std::uint32_t left, std::uint32_t top)
        {
            m_left = static_cast<uint16_t>(left);
            m_top = static_cast<uint16_t>(top);
        }

        inline void set_depth(float min_z, float max_z)
        {
            m_min_z = min_z;
            m_max_z = max_z;
        }
        
        private:

        float   m_min_z;
        float   m_max_z;

        std::uint16_t m_left;
        std::uint16_t m_top;

        std::uint16_t m_width;
        std::uint16_t m_height;
    };
}

#endif