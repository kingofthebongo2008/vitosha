#ifndef __DX11_ERROR_H__
#define __DX11_ERROR_H__

#include <dx11/dx11_exception.h>

namespace dx11
{
    template < typename Exception > void throw_if_failed(HRESULT hr)
    {
        if (hr != S_OK)
        {
            throw Exception();
        }
    }

}


#endif