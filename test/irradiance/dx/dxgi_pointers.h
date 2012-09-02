#ifndef __DXGI_POINTERS_H__
#define __DXGI_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <DXGI.h>

namespace dxgi
{
    typedef boost::intrusive_ptr<IDXGIFactory>				idxgifactory_ptr;
    typedef boost::intrusive_ptr<IDXGIFactory1>				idxgifactory1_ptr;

    typedef boost::intrusive_ptr<IDXGIAdapter>				idxgiadapter_ptr;
    typedef boost::intrusive_ptr<IDXGISwapChain>			idxgiswapchain_ptr;

	typedef boost::intrusive_ptr<IDXGISurface>				idxgisurface_ptr;

}

#endif

