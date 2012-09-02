#ifndef __dxgi_helpers_h__
#define __dxgi_helpers_h__

#include <cstdint>

#include <DXGI.h>

#include <dx11/dx11_pointers.h>
#include <dx11/dx11_error.h>

namespace dxgi
{
    inline dx11::id3d11texture2d_ptr get_buffer(IDXGISwapChain* swap_chain)
    {
        dx11::id3d11texture2d_ptr	result;
        dx11::throw_if_failed< dx11::d3d11_exception> ( swap_chain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) dx11::get_pointer(result) ) );
        return result;
    }

    inline DXGI_SWAP_CHAIN_DESC get_desc(IDXGISwapChain* swap_chain)
    {
        DXGI_SWAP_CHAIN_DESC desc = {};

		dx11::throw_if_failed<dx11::d3d11_exception>( swap_chain->GetDesc(&desc) );
        return desc;
    }
}


#endif

