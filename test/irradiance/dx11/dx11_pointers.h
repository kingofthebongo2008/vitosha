#ifndef __dx11_POINTERS_H__
#define __dx11_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <D3D11.h>

namespace dx11
{
    typedef boost::intrusive_ptr<IDXGIFactory> idxgifactory_ptr;
    typedef boost::intrusive_ptr<IDXGIFactory1> idxgifactory1_ptr;

    typedef boost::intrusive_ptr<IDXGIAdapter> idxgiadapter_ptr;
    typedef boost::intrusive_ptr<IDXGISwapChain> idxgiswapchain_ptr;

    typedef boost::intrusive_ptr<ID3D11Device> id3d11device_ptr;
    typedef boost::intrusive_ptr<ID3D11DeviceContext> id3d11devicecontext_ptr;

    typedef boost::intrusive_ptr<ID3D11CommandList> id3d11commandlist_ptr;

    typedef boost::intrusive_ptr<ID3D11Buffer> id3d11buffer_ptr;
}


inline void intrusive_ptr_release( IUnknown* px )
{
    px->Release();
}

inline void intrusive_ptr_add_ref( IUnknown* px )
{
    px->AddRef();
}



#endif

