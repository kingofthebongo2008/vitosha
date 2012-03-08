#ifndef __GX_CONSTANT_BUFFER_HELPER_H__
#define __GX_CONSTANT_BUFFER_HELPER_H__

#include <cstdint>

#include <dx11/dx11_pointers.h>

namespace gx
{
	template <typename type> void constant_buffer_update( ID3D11DeviceContext* context, ID3D11Buffer* buffer, type value )
	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;

		dx11::throw_if_failed<dx11::d3d11_exception>(context->Map( buffer, 0,  D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) ) ;

		type* data = static_cast< type *> (mapped_resource.pData);

		*data = value;

		context->Unmap( buffer, 0);
	}

	template <typename type> void constant_buffer_update( ID3D11DeviceContext* context, ID3D11Buffer* buffer, type* value )
	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;

		dx11::throw_if_failed<dx11::d3d11_exception>(context->Map( buffer, 0,  D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) ) ;

		type* data = static_cast< type *> (mapped_resource.pData);

		*data = *value;

		context->Unmap( buffer, 0);
	}

	template <typename type> void constant_buffer_update( ID3D11DeviceContext* context, dx11::id3d11buffer_ptr buffer, type* value )
	{
		constant_buffer_update( context, buffer.get(), value);
	}

	template <typename type> void constant_buffer_update( dx11::id3d11devicecontext_ptr context, dx11::id3d11buffer_ptr buffer, type* value )
	{
		constant_buffer_update( context.get(), buffer, value);
	}
}

#endif