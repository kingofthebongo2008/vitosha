#ifndef __WIN32_WINDOW_H__
#define __WIN32_WINDOW_H__

#include <boost/noncopyable.hpp>
#include <cstdint>

#include "dx11/dx11_error.h"
#include "dx11/dx11_system.h"

#include "fnd/fnd_world.h"

namespace wnd
{

	class application;

	class window : public boost::noncopyable
	{
		public:
		explicit window(application& application, dx11::system_context system_context) : m_application(application), m_system_context(system_context)
		{

		}

		void resize_window(std::uint32_t width, std::uint32_t height)
		{
			using namespace dx11;
			DXGI_SWAP_CHAIN_DESC desc;

			throw_if_failed<d3d11_exception>(m_system_context.m_swap_chain->GetDesc(&desc));
			throw_if_failed<d3d11_exception>(m_system_context.m_swap_chain->ResizeBuffers(desc.BufferCount, width, height,  desc.BufferDesc.Format, desc.Flags));
		}

		void render()
		{
			//render world
		}

		void set_world()
		{

		}

		void destroy();

		private:
	
		~window();

		application&				m_application;
		dx11::system_context		m_system_context;
	};

}

#endif