#ifndef __d2d_helpers_h__
#define __d2d_helpers_h__

#include <cstdint>

#include <DXGI.h>
#include <d2d1.h>

#include <d2d/d2d_pointers.h>
#include <d2d/d2d_error.h>

namespace d2d
{
    inline ifactory_ptr create_d2d_factory()
    {
		ifactory_ptr result;

		dx::throw_if_failed<d2d_exception> ( D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, dx::get_pointer(result))) ;

        return result;
    }

	inline irendertarget_ptr create_render_target(ifactory_ptr factory, dxgi::idxgisurface_ptr surface)
    {
		irendertarget_ptr result;
		float	dpi_x = 0.0f;
		float	dpi_y = 0.0f;

		factory->GetDesktopDpi(&dpi_x, &dpi_y);

		D2D1_PIXEL_FORMAT pixel_format = D2D1::PixelFormat();

		D2D1_RENDER_TARGET_PROPERTIES properties = 
		{ 
			D2D1_RENDER_TARGET_TYPE_DEFAULT, 
			pixel_format,
			dpi_x,
			dpi_y,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_10
		};

		dx::throw_if_failed<d2d_exception> ( factory->CreateDxgiSurfaceRenderTarget(surface.get(),&properties, dx::get_pointer(result)) );

        return result;
    }
}


#endif

