#ifndef __D2D_POINTERS_H__
#define __D2D_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <d2d/d2d_pointers.h>

#include <d2d1.h>

namespace d2d
{
	typedef boost::intrusive_ptr<ID2D1Factory>				ifactory_ptr;
	typedef boost::intrusive_ptr<ID2D1RenderTarget>			irendertarget_ptr;
}

#endif

