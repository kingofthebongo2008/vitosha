#ifndef __dx11_HELPERS_H__
#define __dx11_HELPERS_H__

#include <cstdint>

#include <dx11/dx11_pointers.h>

namespace dx11
{
	dx11::id3d11buffer_ptr create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );
	dx11::id3d11buffer_ptr create_default_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );

	dx11::id3d11buffer_ptr create_dynamic_vertex_buffer(ID3D11Device* device, uint32_t size );
	dx11::id3d11buffer_ptr create_dynamic_index_buffer(ID3D11Device* device, uint32_t size );

	dx11::id3d11buffer_ptr create_dynamic_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );
	dx11::id3d11buffer_ptr create_dynamic_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );

	dx11::id3d11buffer_ptr create_immutable_vertex_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );
	dx11::id3d11buffer_ptr create_immutable_index_buffer(ID3D11Device* device, const void* initial_data, uint32_t size );

	inline dx11::id3d11buffer_ptr create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_default_vertex_buffer(device, initial_data, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_default_index_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_default_index_buffer(device, initial_data, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_dynamic_vertex_buffer(ID3D11Device* device, size_t size )
	{
		return create_dynamic_vertex_buffer(device, static_cast<uint32_t>(size) );
	}

	inline dx11::id3d11buffer_ptr create_dynamic_index_buffer(ID3D11Device* device, size_t size )
	{
		return create_dynamic_index_buffer(device, static_cast<uint32_t>(size) );
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
}


#endif

