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
        UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

        D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;

        DXGI_SWAP_CHAIN_DESC desc  = create_default_swap_chain_desc(hwnd);

        IDXGIAdapter* adapter = 0;
        IDXGISwapChain* swap_chain = 0;

        ID3D11Device* device = 0;
        ID3D11DeviceContext* context = 0;

        HRESULT hr  = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_HARDWARE, 0, flags, &level , 1, D3D11_SDK_VERSION, &desc, &swap_chain, &device, 0, &context);

        idxgiadapter_ptr		adapter_ptr(adapter,false);
        idxgiswapchain_ptr		swap_chain_ptr(swap_chain,false);

        id3d11device_ptr		device_ptr(device, false);
        id3d11devicecontext_ptr context_ptr(context,false);

        throw_if_failed<create_device_exception>(hr);

        system_context result = {adapter_ptr, swap_chain_ptr, device_ptr, context_ptr, hwnd};
        return result;
    }

    system_context create_system_context(HWND hwnd, system_context context)
    {
        IDXGIFactory1 * factory = 0;
        IDXGISwapChain* swap_chain = 0;

        throw_if_failed<create_dxgi_factory_exception>( CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory) ) );
        idxgifactory_ptr factory_ptr(factory, false);

        DXGI_SWAP_CHAIN_DESC desc = create_default_swap_chain_desc(hwnd);
      
        throw_if_failed<create_swap_chain_exception>( factory_ptr->CreateSwapChain(context.m_device.get(), &desc, &swap_chain) ); 
        idxgiswapchain_ptr		swap_chain_ptr(swap_chain,false);

        system_context result = {context.m_adapter, swap_chain_ptr, context.m_device,  context.m_immediate_context, hwnd};

        return result;
    }

    void shutdown(system_context)
    {

    }
}
