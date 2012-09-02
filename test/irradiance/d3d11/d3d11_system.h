#ifndef __d3d11_SYSTEM_H__
#define __d3d11_SYSTEM_H__

#include <d3d11/d3d11_pointers.h>
#include <dx/dxgi_pointers.h>

namespace d3d11
{
    struct system_context
    {
        dxgi::idxgiadapter_ptr			m_adapter;
        dxgi::idxgiswapchain_ptr		m_swap_chain;
        idevice_ptr						m_device;
        idevicecontext_ptr				m_immediate_context;
        HWND							m_hwnd;
    };

    system_context create_system_context(HWND hwnd);
    system_context create_system_context(HWND hwnd, system_context context);
}


#endif

