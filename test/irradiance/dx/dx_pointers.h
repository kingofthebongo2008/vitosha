#ifndef __DX_POINTERS_H__
#define __DX_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <combaseapi.h>

namespace dx
{
    template<typename T> 
    inline T** get_pointer( boost::intrusive_ptr<T>& ptr )
    {
        return reinterpret_cast<T**>(&(ptr));
    }

	template<typename T> 
	inline void** get_pointer_void( boost::intrusive_ptr<T>& ptr )
    {
        return reinterpret_cast<void**>(&(ptr));
    }
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

