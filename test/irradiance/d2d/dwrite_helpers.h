#ifndef __dwrite_helpers_h__
#define __dwrite_helpers_h__

#include <cstdint>

#include <dx/dx_error.h>
#include <dx/dx_pointers.h>
#include <d2d/dwrite_pointers.h>

namespace dwrite
{
    inline ifactory_ptr create_dwrite_factory()
    {
		ifactory_ptr result;

		dx::throw_if_failed< dx::exception> ( DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&result))) ;

        return result;
    }

	inline itextformat_ptr create_text_format(ifactory_ptr factory)
    {
		itextformat_ptr result;

		dx::throw_if_failed<dx::exception> ( factory->CreateTextFormat(
        L"Courier",
        NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        14.0f,
        L"en-us",
        dx::get_pointer(result)
        ) );

        return result;
    }
}


#endif

