#ifndef __DXGI_POINTERS_H__
#define __DXGI_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <DXGI.h>

namespace dxgi
{
    typedef boost::intrusive_ptr<IDXGIFactory>				ifactory_ptr;
    typedef boost::intrusive_ptr<IDXGIFactory1>				ifactory1_ptr;

    typedef boost::intrusive_ptr<IDXGIAdapter>				iadapter_ptr;
    typedef boost::intrusive_ptr<IDXGISwapChain>			iswapchain_ptr;

	typedef boost::intrusive_ptr<IDXGISurface>				isurface_ptr;

}

#endif

