#ifndef __dx11_HELPERS_H__
#define __dx11_HELPERS_H__

#include <cstdint>

#include <dx11/dx11_pointers.h>
#include <dx11/dx11_error.h>

namespace dx11
{
	dx11::id3d11buffer_ptr create_constant_buffer(ID3D11Device* device, uint32_t size);

	dx11::id3d11buffer_ptr create_default_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );
	dx11::id3d11buffer_ptr create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );

	dx11::id3d11buffer_ptr create_dynamic_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );
	dx11::id3d11buffer_ptr create_dynamic_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );

	dx11::id3d11buffer_ptr create_immutable_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );
	dx11::id3d11buffer_ptr create_immutable_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );

	inline dx11::id3d11buffer_ptr create_constant_buffer(ID3D11Device* device, size_t size)
	{
		return create_constant_buffer( device, static_cast<uint32_t> (size) );
	}

	inline dx11::id3d11buffer_ptr create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_default_vertex_buffer(device, initial_data, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_default_index_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_default_index_buffer(device, initial_data, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_dynamic_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
			return create_dynamic_vertex_buffer(device, initial_data, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_dynamic_index_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_dynamic_index_buffer(device, initial_data, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_immutable_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_immutable_vertex_buffer( device, initial_data, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_immutable_index_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_immutable_index_buffer( device, initial_data, static_cast<uint32_t>(size) );
	}

	struct d3d11_buffer_scope_lock
	{
		d3d11_buffer_scope_lock( ID3D11DeviceContext* context, ID3D11Buffer* buffer) : m_context(context), m_buffer(buffer)
		{
			dx11::throw_if_failed<dx11::d3d11_exception>(context->Map( buffer, 0,  D3D11_MAP_WRITE_DISCARD, 0, &m_mapped_resource) ) ;
		}

		~d3d11_buffer_scope_lock()
		{
			m_context->Unmap(m_buffer, 0);
		}

		D3D11_MAPPED_SUBRESOURCE	m_mapped_resource;
		ID3D11DeviceContext*		m_context;
		ID3D11Buffer*				m_buffer;
	};
}


#endif

