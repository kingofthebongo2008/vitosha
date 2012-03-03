#ifndef __WIN32_WINDOW_H__
#define __WIN32_WINDOW_H__

#include <cstdint>
#include <memory>

#include <boost/noncopyable.hpp>

#include "dx11/dx11_error.h"
#include "dx11/dx11_pointers.h"

#include "fnd/fnd_world.h"

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
		window(application& application, dx11::idxgiswapchain_ptr swap_chain, gx::render_context* render_context ) : 
			m_application(application)
			, m_swap_chain(swap_chain)
			, m_render_context(render_context)
		{

		}

		inline void set_scene( std::shared_ptr<gx::scene> scene )
		{
			m_scene = scene;
		}

		void resize_window(std::uint32_t width, std::uint32_t height);

		void render();

		void destroy();

		private:
	
		~window();

		application&				m_application;
		dx11::idxgiswapchain_ptr    m_swap_chain;
		gx::render_context*			m_render_context;

		std::shared_ptr<gx::scene>	m_scene;
	};

}

#endif