#ifndef __GX_CONSTANT_BUFFER_HELPER_H__
#define __GX_CONSTANT_BUFFER_HELPER_H__

#include <cstdint>

#include <d3d11/d3d11_helpers.h>
#include <d3d11/d3d11_pointers.h>

namespace gx
{
	namespace detail
	{
		typedef d3d11::d3d11_buffer_scope_lock constant_buffer_scope_lock;
	}

	template <typename type> void constant_buffer_update( ID3D11DeviceContext* context, ID3D11Buffer* buffer, type value )
	{
		detail::constant_buffer_scope_lock lock(context, buffer);
		auto data = static_cast< type *> (lock.m_mapped_resource.pData);
		*data = value;
	}

	template <typename type> void constant_buffer_update( ID3D11DeviceContext* context, ID3D11Buffer* buffer, type* value )
	{
		detail::constant_buffer_scope_lock lock(context, buffer);
		auto data = static_cast< type*> (lock.m_mapped_resource.pData);
		*data = *value;
	}

	inline	void constant_buffer_update( ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* value, size_t size )
	{
		detail::constant_buffer_scope_lock lock(context, buffer);
		auto data = static_cast< void*> (lock.m_mapped_resource.pData);
		::memcpy(data, value,  size  );
	}
}

#endif