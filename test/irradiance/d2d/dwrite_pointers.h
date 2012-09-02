#ifndef __DWRITE_POINTERS_H__
#define __DWRITE_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <d2d/d2d_pointers.h>
#include <d2d1.h>

namespace dwrite
{
	typedef boost::intrusive_ptr<ID2D1Factory>				idwritetext_ptr;

}


#endif

