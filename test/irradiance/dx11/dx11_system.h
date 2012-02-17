#ifndef __dx11_SYSTEM_H__
#define __dx11_SYSTEM_H__

#include <dx11/dx11_pointers.h>

namespace dx11
{
    struct system_context
    {
        idxgiadapter_ptr			m_adapter;
        idxgiswapchain_ptr			m_swap_chain;
        id3d11device_ptr			m_device;
        id3d11devicecontext_ptr		m_immediate_context;
        HWND						m_hwnd;
    };

    system_context create_system_context(HWND hwnd);
    system_context create_system_context(HWND hwnd, system_context context);
}


#endif

