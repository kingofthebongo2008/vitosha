#ifndef __IO_MOUSE_H__
#define __IO_MOUSE_H__

#include <cstdint>
#include <windows.h>

#include <math/math_functions.h>
#include <util/util_bits.h>

namespace io
{
	class mouse_state
	{
		public:

		enum mask : uint8_t
		{
			left_button = 0x0,
			middle_button = 0x1,
			right_button = 0x2
		};

		typedef uint8_t difference;

		mouse_state() : m_state(0), m_shadow_state(0)
		{

		}

		void swap()
		{
			m_shadow_state = m_state;
		}

		difference get_difference() const
		{
			return m_state ^ m_shadow_state;
		}

		bool is_left_button_down() const
		{
			return util::bit_is_set<left_button, uint8_t>(m_state);
		}

		void set_left_button_down()
		{
			m_state = util::bit_set<left_button, uint8_t>(m_state);
		}

		void set_left_button_up()
		{
			m_state = util::bit_reset<left_button, uint8_t>(m_state);
		}

		bool is_right_button_down() const
		{
			return util::bit_is_set<right_button, uint8_t>(m_state);
		}

		void set_right_button_down()
		{
			m_state = util::bit_set<right_button, uint8_t>(m_state);
		}

		void set_right_button_up()
		{
			m_state = util::bit_reset<right_button, uint8_t>(m_state);
		}

		bool is_middle_button_down() const
		{
			return util::bit_is_set<middle_button, uint8_t>(m_state);
		}

		void set_middle_button_down()
		{
			m_state = util::bit_set<middle_button, uint8_t>(m_state);
		}

		void set_middle_button_up()
		{
			m_state = util::bit_reset<middle_button, uint8_t>(m_state);
		}

		void set_coordinates(uint32_t x, uint32_t y)
		{
			m_x = static_cast<uint16_t>(x);
			m_y = static_cast<uint16_t>(y);
		}

		std::tuple<uint32_t, uint32_t> get_coordinates() const
		{
			return std::make_tuple<uint32_t, uint32_t> ( m_x, m_y);
		}

		void set_state(uint32_t state)
		{
			m_state = static_cast<uint8_t>(state);
		}

		void set_mask(uint32_t mask)
		{
			m_state = m_state | static_cast<uint8_t> (mask);
		}

		void reset_mask(uint32_t mask)
		{
			m_state = m_state & ( ~(mask) );
		}

		private:

		uint16_t m_x;
		uint16_t m_y;

		uint8_t	m_state;
		uint8_t m_shadow_state;
	};

	namespace details
	{
		template < uint32_t mouse_button, uint32_t windows_mouse_button > inline uint32_t create_button_state(uint32_t windows_mouse_state)
		{
			uint32_t v_1 = windows_mouse_state & windows_mouse_button;
			uint32_t v_2 = math::log2_c<windows_mouse_button>::value;
			uint32_t v_3 = mouse_button ;
			return  (v_1 >> v_2) << v_3 ;
		}
	}

	inline uint32_t create_mouse_state(uint32_t windows_mouse_state)
	{
		uint32_t result = 0;

		result = details::create_button_state<mouse_state::left_button, MK_LBUTTON> (windows_mouse_state);
		result = result | details::create_button_state<mouse_state::right_button, MK_RBUTTON> (windows_mouse_state); 
		result = result | details::create_button_state<mouse_state::middle_button, MK_MBUTTON> (windows_mouse_state);

		return result;
	}

};


#endif