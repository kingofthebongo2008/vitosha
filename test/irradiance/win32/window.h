#ifndef __WIN32_WINDOW_H__
#define __WIN32_WINDOW_H__

#include <cstdint>
#include <memory>

#include <boost/noncopyable.hpp>

#include "dx11/dx11_error.h"
#include "dx11/dx11_pointers.h"

#include "io/io_mouse.h"
#include "fnd/fnd_world.h"

#include "gx/gx_pinhole_camera.h"

namespace gx
{
	class scene;
	class render_context;
}

namespace wnd
{
    class application;


	class window : private boost::noncopyable
	{
		public:

		window(application& application, dx11::idxgiswapchain_ptr swap_chain, gx::render_context* render_context );

		inline void set_scene( std::shared_ptr<gx::scene> scene )
		{
			m_scene = scene;
		}

		void resize_window(std::uint32_t width, std::uint32_t height);

		void process_user_input();

		void render();
		void destroy();

		void on_mouse_down(uint32_t windows_mouse_state);
		void on_mouse_up(uint32_t windows_mouse_state);
		void on_mouse_move(uint32_t windows_mouse_state, uint16_t x, uint16_t y);

		private:

		~window();

		application&				m_application;
		dx11::idxgiswapchain_ptr    m_swap_chain;
		gx::render_context*			m_render_context;
		std::shared_ptr<gx::scene>	m_scene;
		bool						m_occluded_by_another_window;

		gx::pinhole_camera			m_main_camera;

		io::mouse_state				m_mouse_state;

		void render_frame();
	};

}

#endif