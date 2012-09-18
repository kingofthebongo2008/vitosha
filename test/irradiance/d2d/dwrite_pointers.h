#ifndef __DWRITE_POINTERS_H__
#define __DWRITE_POINTERS_H__

#include <boost/intrusive_ptr.hpp>

#include <dwrite.h>

namespace dwrite
{
	typedef boost::intrusive_ptr<IDWriteFactory>				ifactory_ptr;
	typedef boost::intrusive_ptr<IDWriteTextFormat>				itextformat_ptr;

}


#endif

