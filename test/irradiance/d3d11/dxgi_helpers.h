#ifndef __dxgi_helpers_h__
#define __dxgi_helpers_h__

#include <cstdint>

#include <DXGI.h>

#include <d3d11/d3d11_pointers.h>
#include <d3d11/d3d11_error.h>

namespace dxgi
{
    inline d3d11::itexture2d_ptr get_buffer(IDXGISwapChain* swap_chain)
    {
        d3d11::itexture2d_ptr	result;
        dx::throw_if_failed< d3d11::d3d11_exception> ( swap_chain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**) dx::get_pointer(result) ) );
        return result;
    }

    inline DXGI_SWAP_CHAIN_DESC get_desc(IDXGISwapChain* swap_chain)
    {
        DXGI_SWAP_CHAIN_DESC desc = {};

		dx::throw_if_failed<d3d11::d3d11_exception>( swap_chain->GetDesc(&desc) );
        return desc;
    }
}


#endif

