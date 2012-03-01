#include "precompiled.h"

#include <dx11/dx11_render_context.h>


namespace dx11
{
    render_context::render_context(system_context sys_context, std::uint32_t thread_render_context_count) : m_system_context(sys_context)
    {
        m_render_contexts.reserve(thread_render_context_count);

        for (std::uint32_t i = 0; i < thread_render_context_count; ++i)
        {
			id3d11devicecontext_ptr device_context;

			dx11::throw_if_failed<dx11::create_deferred_context_exception> ( sys_context.m_device->CreateDeferredContext(0, dx11::get_pointer(device_context)));

			std::unique_ptr<thread_render_context> ptr(new thread_render_context(device_context) );
            m_render_contexts.push_back( std::move(ptr) );
        }
    }

    void render_context::begin_frame()
    {
        for  (auto it = std::begin(m_render_contexts); it!= std::end(m_render_contexts); ++it)
        {
            (*it)->swap();
        }
    }

    void render_context::end_frame()
    {
        for  (auto it = std::begin(m_render_contexts); it!= std::end(m_render_contexts); ++it)
        {
            (*it)->submit(m_system_context.m_immediate_context);
        }

        throw_if_failed<d3d11_exception>(m_system_context.m_swap_chain->Present(0,0));
    }
}
