#ifndef __dx11_RENDER_CONTEXT_H__
#define __dx11_RENDER_CONTEXT_H__

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/noncopyable.hpp>

#include "dx11/dx11_pointers.h"
#include "dx11/dx11_error.h"
#include "dx11/dx11_system.h"

namespace dx11
{
	class thread_render_context;

	class render_context : public boost::noncopyable
	{
		public:
		explicit render_context(system_context sys_context, std::uint32_t render_context_count);
		void begin_frame();
		void end_frame();

		private:
		render_context();
		system_context	m_system_context;
		std::vector< std::unique_ptr<thread_render_context> > m_render_contexts;
	};

	class thread_render_context : public boost::noncopyable
	{
		public:
		explicit thread_render_context(render_context& render_context) : m_render_context(render_context)
		{

		}

		void begin_frame()
		{
			m_d3d11_device_context->ClearState();
		}

		void end_frame()
		{
			throw_if_failed<d3d11_exception>( m_d3d11_device_context->FinishCommandList(FALSE, reinterpret_cast<ID3D11CommandList**> (&m_command_list)));
		}

		void submit( id3d11devicecontext_ptr immediate_context)
		{
			if (m_command_list)
			{
				immediate_context->ExecuteCommandList(m_command_list.get(), FALSE);
				m_command_list.reset();
			}
		}

		void reset()
		{
			m_command_list.reset();
			m_command_list_submit.reset();
		}

		void swap()
		{
			m_command_list_submit.swap( m_command_list );
		}

		private:
		render_context&			m_render_context;
		id3d11devicecontext_ptr m_d3d11_device_context;

		id3d11commandlist_ptr	m_command_list;
		id3d11commandlist_ptr	m_command_list_submit;
	};
}



#endif

