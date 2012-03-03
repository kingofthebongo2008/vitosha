#include "precompiled.h"

#include <dx11/dx11_system.h>

#include <DXGI.h>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>


namespace dx11
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
        UINT flags					= D3D11_CREATE_DEVICE_DEBUG;

        D3D_FEATURE_LEVEL level		= D3D_FEATURE_LEVEL_11_0;
        DXGI_SWAP_CHAIN_DESC desc	= create_default_swap_chain_desc(hwnd);

        idxgiadapter_ptr			adapter;
        idxgiswapchain_ptr			swap_chain;

        id3d11device_ptr			device;
        id3d11devicecontext_ptr		context;

        HRESULT hr  = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, &level , 1, D3D11_SDK_VERSION, &desc, get_pointer(swap_chain), get_pointer(device), 0, get_pointer(context) );

        throw_if_failed<create_device_exception>(hr);

        system_context result = {adapter, swap_chain, device, context, hwnd};
        return result;
    }

    system_context create_system_context(HWND hwnd, system_context context)
    {
        idxgifactory1_ptr factory;

        throw_if_failed<create_dxgi_factory_exception>( CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**) get_pointer(factory)  ) );

        DXGI_SWAP_CHAIN_DESC desc = create_default_swap_chain_desc(hwnd);
      
        idxgiswapchain_ptr		swap_chain;
        throw_if_failed<create_swap_chain_exception>( factory->CreateSwapChain(context.m_device.get(), &desc,  get_pointer(swap_chain) ) ); 
        
        system_context result = {context.m_adapter, swap_chain, context.m_device,  context.m_immediate_context, hwnd};

        return result;
    }

    void shutdown(system_context)
    {

    }
}
