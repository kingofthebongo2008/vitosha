#ifndef __GX_CONSTANT_BUFFER_HELPER_H__
#define __GX_CONSTANT_BUFFER_HELPER_H__

#include <cstdint>

#include <dx11/dx11_pointers.h>

namespace gx
{
	namespace detail
	{
		struct constant_buffer_scope_lock
		{
			constant_buffer_scope_lock( ID3D11DeviceContext* context, ID3D11Buffer* buffer) : m_context(context), m_buffer(buffer)
			{
				dx11::throw_if_failed<dx11::d3d11_exception>(context->Map( buffer, 0,  D3D11_MAP_WRITE_DISCARD, 0, &m_mapped_resource) ) ;
			}

			~constant_buffer_scope_lock()
			{
				m_context->Unmap(m_buffer, 0);
			}

			D3D11_MAPPED_SUBRESOURCE	m_mapped_resource;
			ID3D11DeviceContext*		m_context;
			ID3D11Buffer*				m_buffer;
		};

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