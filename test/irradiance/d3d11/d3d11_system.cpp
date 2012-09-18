#include "precompiled.h"

#include <d3d11/d3d11_system.h>

#include <DXGI.h>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>


namespace d3d11
{
    namespace
    {
        static DXGI_SWAP_CHAIN_DESC create_default_swap_chain_desc(HWND hwnd)
        {
            DXGI_MODE_DESC mode = {};
            DXGI_SWAP_CHAIN_DESC desc  = {};

            mode.RefreshRate.Numerator = 60;
            mode.RefreshRate.Denominator = 1;
            mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

            desc.BufferDesc = mode;
            desc.Windowed = (hwnd !=0);
            desc.OutputWindow = hwnd;
            desc.BufferCount = 1;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            //desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			desc.Flags = 0;
            return desc;
        }
    }
    system_context create_system_context(HWND hwnd)
    {
        auto flags					= D3D11_CREATE_DEVICE_DEBUG |  D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        auto level					= D3D_FEATURE_LEVEL_11_0;
        auto desc					= create_default_swap_chain_desc(hwnd);

        dxgi::iadapter_ptr			adapter;
        dxgi::iswapchain_ptr		swap_chain;

        idevice_ptr						device;
        idevicecontext_ptr				context;

        auto hr  = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, &level , 1, D3D11_SDK_VERSION, &desc, dx::get_pointer(swap_chain), dx::get_pointer(device), 0, dx::get_pointer(context) );

        dx::throw_if_failed<create_device_exception>(hr);

        system_context result = {adapter, swap_chain, device, context, hwnd};
        return result;
    }

    system_context create_system_context(HWND hwnd, system_context context)
    {
        dxgi::ifactory1_ptr factory;

        dx::throw_if_failed<create_dxgi_factory_exception>( CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**) dx::get_pointer(factory)  ) );

        DXGI_SWAP_CHAIN_DESC desc = create_default_swap_chain_desc(hwnd);
      
        dxgi::iswapchain_ptr		swap_chain;
        dx::throw_if_failed<create_swap_chain_exception>( factory->CreateSwapChain(context.m_device.get(), &desc,  dx::get_pointer(swap_chain) ) ); 
        
        system_context result = {context.m_adapter, swap_chain, context.m_device,  context.m_immediate_context, hwnd};

        return result;
    }

    void shutdown(system_context)
    {

    }
}
