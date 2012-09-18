#ifndef __DX_ERROR_H__
#define __DX_ERROR_H__

#include <exception>

namespace dx
{
    template < typename exception > void throw_if_failed(HRESULT hr)
    {
        if (hr != S_OK)
        {
            throw exception();
        }
    }


	class exception : public std::exception
    {

    };
}


#endif