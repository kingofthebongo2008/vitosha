#ifndef __DX_ERROR_H__
#define __DX_ERROR_H__

namespace dx
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