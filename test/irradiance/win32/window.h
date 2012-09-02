#ifndef __WIN32_WINDOW_H__
#define __WIN32_WINDOW_H__

#include <cstdint>
#include <memory>

#include <boost/noncopyable.hpp>

#include "dx/dxgi_pointers.h"

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

#include "fnd/fnd_world.h"

#include "io/io_mouse.h"
#include "io/io_pad.h"


#include "gx/gx_pinhole_camera.h"
#include "gx/gx_view_port.h"

namespace gx
{
	class scene;
	class render_context;
}

namespace wnd
{
    class application;

	struct arcball_rotation_state
	{
			math::float4 initial_rotation;
			math::float4 initial_point;
			math::float4 current_rotation;
			math::float4 initial_camera_up;
			math::float4 initial_camera_direction;
			math::float4 initial_center;
			bool		 dragging;

			arcball_rotation_state()
			{
				initial_rotation = math::identity_r3();
				current_rotation = math::identity_r3();
				initial_point	 = math::identity_r3();

				initial_camera_up	= math::zero();
				initial_camera_direction = math::zero();
				initial_center = math::identity_r3();

				dragging = false;
			}
	};


	class window : private boost::noncopyable
	{
		public:

		window(application& application, dxgi::idxgiswapchain_ptr swap_chain, gx::render_context* render_context );

		inline void set_scene( std::shared_ptr<gx::scene> scene )
		{
			m_scene = scene;
		}

		void resize_window(uint32_t width, uint32_t height);

		void process_user_input();

		void render();
		void destroy();

		void on_mouse_down(uint32_t windows_mouse_state);
		void on_mouse_up(uint32_t windows_mouse_state);
		void on_mouse_move(uint32_t windows_mouse_state, uint16_t x, uint16_t y);

		void on_button_0_down();
		void on_button_1_down();
		void on_button_2_down();
		void on_button_3_down();
		void on_button_4_down();
		void on_button_5_down();
		void on_button_6_down();

		void on_button_0_up();
		void on_button_1_up();
		void on_button_2_up();
		void on_button_3_up();
		void on_button_4_up();
		void on_button_5_up();
		void on_button_6_up();

	private:

		~window();

		application&				m_application;
		dxgi::idxgiswapchain_ptr    m_swap_chain;
		gx::render_context*			m_render_context;
		std::shared_ptr<gx::scene>	m_scene;
		bool						m_occluded_by_another_window;

		gx::pinhole_camera			m_main_camera;
		gx::view_port				m_view_port;

		io::mouse_state				m_mouse_state;
		io::pad_state				m_pad_state;

		arcball_rotation_state		m_arcball_state;

		void render_frame();
	};

}

#endif